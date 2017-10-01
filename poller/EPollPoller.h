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
		virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels);
		virtual void updateChannel(Channel* channel);
		virtual void removeChannel(Channel* channel);
	private:
		static const int kInitEventListSize = 16;
		void fillActiveChannels(int numEvents,
								ChannelList* activeChannels)const;
		typedef std::vector<struct epoll_event> EventList;
		int _epollfd;
		EventList _events;
};

#endif
