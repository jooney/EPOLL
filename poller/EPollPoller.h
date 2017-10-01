#ifndef _EPOLLPOLLER_H__
#define _EPOLLPOLLER_H__

#include "Poller.h"
#include <vector>
  
struct epoll_event;

class EPollPoller : public Poller
{
	public:
		EPollPoller(EventLoop* loop);
		virtual ~EPollPoller();
	private:
		static const int kInitEventListSize = 16;
		typedef std::vector<struct epoll_event> EventList;
		int _epollfd;
		EventList _events;
};

#endif
