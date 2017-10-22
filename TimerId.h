#ifndef __TIMERID_H__
#define __TIMERID_H__

#include <iostream>
class Timer;
class TimerId
{
	public:
		TimerId()
			:_timer(NULL),
			_sequence(0)
	{}
		TimerId(Timer* timer, int64_t seq)
			:_timer(timer),
			_sequence(seq)
	{}
		friend class TimeQueue;
	private:
		Timer*     _timer;
		int64_t    _sequence;
};



#endif
