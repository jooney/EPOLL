#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop* loop)
	:_ownerLoop(loop)
{
}

Poller::~Poller(){}

bool Poller::hasChannel(Channel* channel)const
{
	ChannelMap::const_iterator it = _channels.find(channel->fd());
	return it != _channels.end() && it->second == channel;
}
