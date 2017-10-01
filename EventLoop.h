/*
 * EventLoop.h
 *
 *  Created on: 2017Äê9ÔÂ23ÈÕ
 *      Author: jjz
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "CurrentThread.h"
#include "Timestamp.h"
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <functional>
#include <vector>
using namespace muduo;
class Channel;
class Poller;
class EventLoop : boost::noncopyable
{
public:
	typedef std::function<void()> Functor;	
	EventLoop();
	~EventLoop();
	void loop();
	void quit();
	void runInLoop(const Functor& cb);
	void updateChannel(Channel* channel);
	bool hasChannel(Channel* channel);
	static EventLoop* getEventLoopOfCurrentThread();
	void assertInLoopThread()
	{
		if (!isInLoopThread())
		{
			abortNotInLoopThread();
		}
	}
	bool isInLoopThread() const {return _threadId == CurrentThread::tid();}
	//static EventLoop* getEventLoopOfCurrentThread();

private:
	void abortNotInLoopThread();
	typedef std::vector<Channel*> ChannelList;
	bool _looping;
	bool _quit;
	//static EventLoop* t_loopInThisThread;
	const pid_t   _threadId;
	int _wakeupFd;
//	boost::scoped_ptr<Channel> _wakeupChannel;
	boost::scoped_ptr<Poller> _poller;
	ChannelList _activeChannels;
	Channel*    _currentActiveChannel;
	std::vector<Functor> _pendingFunctors;

};



#endif /* EVENTLOOP_H_ */
