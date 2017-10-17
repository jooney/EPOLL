#include "Poller.h"
#include "EPollPoller.h"

Poller* Poller::newDefaultPoller(EventLoop* loop,ZQ::common::Log& log)
{
	return new EPollPoller(loop,log);
}

