#include "EPollPoller.h"
#include "Logging.h"
#include "Channel.h"
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
		fillActiveChannels(numEvents,activeChannels);
	}
	return now;
}

void EPollPoller::fillActiveChannels(int numEvents,
									ChannelList* activeChannels)const
{
	assert(implicit_cast<size_t>(numEvents)<= _events.size());
	for (int i = 0;i< numEvents;i++)
	{
		Channel* channel = static_cast<Channel*>(_events[i].data.ptr);
		int fd = channel->fd();
		ChannelMap::const_iterator it = _channels.find(fd);
	}
}
void EPollPoller::updateChannel(Channel* channel)
{
	Poller::assertInLoopThread();	
	//if channel is new ,then the index = -1(Channel's constructor)
	const int index = channel->index();
	LOG_TRACE << "fd = " << channel->fd()<<" events = "<<channel->events()<<" index = "<<index;
	if (index == kNew || index == kDeleted)
	{
		//a new one fd , add with EPOLL_CTL_ADD
		int fd = channel->fd();
		if (index == kNew)	
		{
			assert(_channels.find(fd) == _channels.end());
			_channels[fd] = channel;
		}
		else // index == kDeleted
		{
			assert(_channels.find(fd) != _channels.end());
			assert(_channels[fd] == channel);
		}
		channel->set_index(kAdded);
	}

}

void EPollPoller::removeChannel(Channel* channel)
{
}

void EPollPoller::update(int operation, Channel* channel)
{
	struct epoll_event event;
	bzero(&event,sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();
	LOG_TRACE << "epoll_ctl op = "<<operationToString(operation)
		<<" fd = "<<fd<<" event = ( "<<channel->eventsToString() << " )";
}

const char* EPollPoller::operationToString(int op)
{
	switch (op)
	{
		case EPOLL_CTL_ADD:
			return "ADD";
		case EPOLL_CTL_DEL:
			return "DEL";
		case EPOLL_CTL_MOD:
			return "MOD";
		default:
			assert(false && "ERROR op");
			return "Unknown Operation";
	}
}
