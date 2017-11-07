/*
 * EventLoop.h
 *
 *  Created on: 2017��9��23��
 *      Author: jjz
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "Mutex.h"
#include "CurrentThread.h"
#include "Timestamp.h"
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <functional>
#include <vector>
using namespace muduo;
class Channel;
class Poller;
class TimerQueue;
class EventLoop : boost::noncopyable
{
public:
	typedef std::function<void()> Functor;	
	EventLoop();
	~EventLoop();
	void loop();
	void quit();
	void runInLoop(const Functor& cb);
	void queueInLoop(const Functor& cb);
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);
	bool hasChannel(Channel* channel);
	void wakeup();
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
	void printActiveChannels() const;
	void doPendingFunctors();
	typedef std::vector<Channel*> ChannelList;
	bool _looping;
	bool _quit;
	bool _eventHandling;
	bool _callingPendingFunctors;
	//static EventLoop* t_loopInThisThread;
	const pid_t   _threadId;
	Timestamp     _pollReturnTime;
	int _wakeupFd;
//	boost::scoped_ptr<Channel> _wakeupChannel;
	boost::scoped_ptr<Poller> _poller;
//	boost::scoped_ptr<TimerQueue> _timeQueue;
	ChannelList _activeChannels;
	Channel*    _currentActiveChannel;

	mutable MutexLock    _mutex;
	std::vector<Functor> _pendingFunctors;

};



#endif /* EVENTLOOP_H_ */
