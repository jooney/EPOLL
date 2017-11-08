#include "TimerQueue.h"
#include "EventLoop.h"
#include "Logging.h"
#include <sys/timerfd.h>
#include "TimerId.h"
#include "Timer.h"

int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
								TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0)
	{
		LOG_SYSFATAL << "Failed in timerfd_create";
	}
	return timerfd;
}

void readTimerfd(int timerfd,Timestamp now)
{
	uint64_t howmany;
	ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
	LOG_TRACE << "TimerQueue::handleRead() "<<howmany<<" at "<<now.toString();
	if (n!= sizeof howmany)
	{
		LOG_ERROR<<"TimerQueue::handleRead() reads "<<n<<" bytes instead of 8";
	}
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
	int64_t microseconds = when.microSecondsSinceEpoch()
							- Timestamp::now().microSecondsSinceEpoch();
	if (microseconds < 100)
	{
		microseconds = 100;
	}
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(
			microseconds / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>(
			(microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
	return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue, sizeof newValue);
	bzero(&oldValue, sizeof oldValue);
	newValue.it_value = howMuchTimeFromNow(expiration);
	int ret = ::timerfd_settime(timerfd,0,&newValue,&oldValue);
	if (ret)
	{
		LOG_SYSERR << "timerfd_settime()";
	}
}

TimerQueue::TimerQueue(EventLoop* loop)
	:_loop(loop),
	_timerfd(createTimerfd()),
	_timerfdChannel(loop,_timerfd),
	_timers(),
	_callingExpiredTimers(false)
{
	_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead,this));
	_timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
	_timerfdChannel.disableAll();	
	_timerfdChannel.remove();
	::close(_timerfd);
}
// when: expiration for Timer
TimerId TimerQueue::addTimer(const TimerCallback& cb,
							Timestamp when,
							double interval)
{
	Timer* timer = new Timer(cb,when,interval);
	_loop->runInLoop(
			std::bind(&TimerQueue::addTimerInLoop,this,timer));
	return TimerId(timer,timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
	_loop->runInLoop(std::bind(&TimerQueue::cancelInLoop,this,timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
	_loop->assertInLoopThread();
	assert(_timers.size() == _activeTimers.size());
	ActiveTimer timer(timerId._timer, timerId._sequence);
	ActiveTimerSet::iterator it = _activeTimers.find(timer);
	if (it != _activeTimers.end())
	{
		size_t n = _timers.erase(Entry(it->first->expiration(),it->first));
		assert(n == 1);(void)n;
		delete it->first;
		_activeTimers.erase(it);
	}
	else if (_callingExpiredTimers)
	{
		_cancelingTimers.insert(timer);
	}
	assert(_timers.size() == _activeTimers.size());
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	_loop->assertInLoopThread();
	bool earliestChanged = insert(timer);
	if (earliestChanged)
	{
		resetTimerfd(_timerfd,timer->expiration());
	}
}

bool TimerQueue::insert(Timer* timer)
{
	_loop->assertInLoopThread();
	assert(_timers.size() == _activeTimers.size());
	bool earliestChanged = false;
	Timestamp when = timer->expiration();
	TimerList::iterator it = _timers.begin();
	if ( it == _timers.end() || when < it->first)
	{
		earliestChanged = true;
	}
	{
		std::pair<TimerList::iterator, bool> ret
			= _timers.insert(Entry(when,timer));
		assert(ret.second); (void)ret;
	}
	{
		std::pair<ActiveTimerSet::iterator , bool>ret
			= _activeTimers.insert(ActiveTimer(timer,timer->sequence()));
		assert(ret.second);(void)ret;
	}
	assert(_timers.size() == _activeTimers.size());
	return earliestChanged;
}

void TimerQueue::handleRead()
{
	_loop->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(_timerfd,now);
	std::vector<Entry> expired = getExpired(now);
	_callingExpiredTimers = true;
	_cancelingTimers.clear();
	for (std::vector<Entry>::iterator it = expired.begin();
		it != expired.end();++it)
	{
		it->second->run();
	}
	_callingExpiredTimers = false;
	//reset(expired,now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	assert(_timers.size() == _activeTimers.size());
	std::vector<TimerQueue::Entry> expired;
	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = _timers.lower_bound(sentry);
	assert(end == _timers.end() || now < end->first);
	std::copy(_timers.begin(),end,back_inserter(expired));
	for (std::vector<Entry>::iterator it = expired.begin();
		it != expired.end();++it)
	{
		ActiveTimer timer(it->second, it->second->sequence());
		size_t n = _activeTimers.erase(timer);
		assert(n == 1); (void)n;
	}
	assert(_timers.size() == _activeTimers.size());
	return expired;
}

//if timer is repeat , so put back to _timers 
void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
	Timestamp nextExpire;
	for (std::vector<Entry>::const_iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		ActiveTimer timer(it->second, it->second->sequence());
		if (it->second->repeat()
			&& _cancelingTimers.find(timer) == _cancelingTimers.end())
		{
			it->second->restart(now);
			insert(it->second);
		}
		else
		{
			delete it->second;
		}
	}
	if (!_timers.empty())
	{
		nextExpire = _timers.begin()->second->expiration();
	}
	if (nextExpire.valid())
	{
		resetTimerfd(_timerfd,nextExpire);
	}
}

