#include "EPollPoller.h"
#include "Logging.h"
#include <sys/epoll.h>
#include <poll.h>
#include <errno.h>
#include <boost/static_assert.hpp>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);

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
	LOG_TRACE<<"fd total count "<<_channels.size();
	int numEvents = ::epoll_wait(_epollfd,
								&(*_events.begin()),
								static_cast<int>(_events.size()), //initial size:16
								timeoutMs); //timeout: 10000
	int savedErrno = errno;
	Timestamp now(Timestamp::now());
	if (numEvents >0)
	{
		LOG_TRACE<<numEvents<<" events happened";

	}
	return now;
}

void EPollPoller::fillActiveChannels(int numEvents,
									ChannelList* activeChannels)const
{
}
void EPollPoller::updateChannel(Channel* channel)
{

}

void EPollPoller::removeChannel(Channel* channel)
{
}
