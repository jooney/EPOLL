/*
 * EventLoop.h
 *
 *  Created on: 2017Äê9ÔÂ23ÈÕ
 *      Author: jjz
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "base/CurrentThread.h"
#include "base/Timestamp.h"
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <vector>
class Channel;
class EventLoop
{
public:
	typedef std::function<void()> Functor;	
	EventLoop();
	~EventLoop();
	void loop();
	void quit();
	void runInLoop();
	void updateChannel(Channel* channel);
	bool hasChannel(Channel* channel);
	static EventLoop* getEventLoopOfCurrentThread();
	void assertInLoopThread()
	{
	}
	//void assertInLoopThread();
	//static EventLoop* getEventLoopOfCurrentThread();

private:
	//bool isInLoopThread();
	//void abortNotInLoopThread();
	typedef std::vector<Channel*> ChannelList;
	bool _looping;
	bool _quit;
	//static EventLoop* t_loopInThisThread;
	const pid_t   _threadId;
	int _wakeupFd;
//	boost::scoped_ptr<Channel> _wakeupChannel;
	ChannelList _activeChannels;
	Channel*    _currentActiveChannel;
	std::vector<Functor> _pendingFunctors;

};



#endif /* EVENTLOOP_H_ */
