#include "EPollPoller.h"
#include "Logging.h"
#include <sys/epoll.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
	:Poller(loop),
	_epollfd(::epoll_create(EPOLL_CLOEXEC)),
	_events(kInitEventListSize)
{
	if (_epollfd < 0)
	{
		LOG_SYSFATAL << "EPollPoller::EPollPoller";
	}
}
EPollPoller::~EPollPoller()
{
	::close(_epollfd);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
	Timestamp now(Timestamp::now());
	return now;
}

void EPollPoller::updateChannel(Channel* channel)
{

}

void EPollPoller::removeChannel(Channel* channel)
{
}
