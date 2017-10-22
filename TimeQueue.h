#ifndef __TIMEQUEUE_H__
#define __TIMEQUEUE_H__

#include "Timestamp.h"
#include "Callbacks.h"

class EventLoop;
class Timer;
class TimerId;

class TimerQueue
{
	public:
		TimeQueue(EventLoop* loop);
		~TimeQueue();
		TimerId addTimer(const TimerCallback& cb,
						Timestamp when,
						double interval);
		void cancel(TimerId timerId);

	private:
		typedef std::pair<Timestamp, Timer*> Entry;
		typedef std::set<Entry> TimerList;
		typedef std::pair<Timer*, int64_t> ActiveTimer;
		typedef std::set<ActiveTimer> ActiveTimerSet;
	
};



#endif
