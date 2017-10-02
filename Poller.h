#ifndef __POLLER_H__
#define __POLLER_H__
#include "Timestamp.h"
#include "EventLoop.h"
#include <vector>
#include <map>
class Channel;
using namespace muduo;
class Poller
{
	public:
		typedef std::vector<Channel*> ChannelList;
		Poller(EventLoop* loop);
		virtual ~Poller();
		virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels) = 0;
		virtual void updateChannel(Channel* channel) = 0;
		virtual void removeChannel(Channel* channel) = 0;
		virtual bool hasChannel(Channel*channel) const;
		static Poller* newDefaultPoller(EventLoop* loop);
		void assertInLoopThread() const
		{
			_ownerLoop->assertInLoopThread();
		}
	protected:
		typedef std::map<int,Channel*>ChannelMap;
		ChannelMap _channels;
	private:
		EventLoop* _ownerLoop;
};
#endif
