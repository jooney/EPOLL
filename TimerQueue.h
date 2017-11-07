#ifndef __TIMEQUEUE_H__
#define __TIMEQUEUE_H__

#include <vector>
#include <set>
#include "Timestamp.h"
#include "Callbacks.h"
#include "Channel.h"

class EventLoop;
class Timer;
class TimerId;

class TimerQueue
{
	public:
		TimerQueue(EventLoop* loop);
		~TimerQueue();
		TimerId addTimer(const TimerCallback& cb,
						Timestamp when,
						double interval);
		void cancel(TimerId timerId);

	private:
		typedef std::pair<Timestamp, Timer*> Entry;
		typedef std::set<Entry> TimerList;
		typedef std::pair<Timer*, int64_t> ActiveTimer;
		typedef std::set<ActiveTimer> ActiveTimerSet;

		void  addTimerInLoop(Timer* timer);
		void  cancelInLoop(TimerId timerId);
		void  handleRead();

		bool  insert(Timer* timer);

		std::vector<Entry> getExpired(Timestamp now);
		
		EventLoop*   _loop;
		const int    _timerfd;
		Channel      _timerfdChannel;
		TimerList     _timers;

		ActiveTimerSet    _activeTimers;
		bool              _callingExpiredTimers;
		ActiveTimerSet    _cancelingTimers;
};



#endif
