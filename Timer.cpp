#include "Timer.h"

AtomicInt64 Timer::_s_numCreated; 

void Timer::restart(Timestamp now)
{
	if (_repeat)
	{
		_expiration = addTime(now, _interval);
	}
	else
	{
		_expiration = Timestamp::invalid();
	}
}
