/*
 * EventLoop.cpp
 *
 *  Created on: 2017Äê9ÔÂ23ÈÕ
 *      Author: jjz
 */
#include "Logging.h"
#include "EventLoop.h"
#include "Channel.h"
#include "iostream"
#include <unistd.h>
#include <assert.h>

#define LOG_FUNC() printf("func here %s\n",__FUNCTION__)

__thread EventLoop* t_loopInThisThread = 0;

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	:_looping(false),
	 _quit(false),
	 _threadId(CurrentThread::tid())
{
	LOG_DEBUG << "EventLoop created"<<this<<" in thread "<<_threadId;
	if (t_loopInThisThread)
	{
		LOG_FATAL << "Another EventLoop "<<t_loopInThisThread
			      << " exists in this thread "<<_threadId;
	}
	else
	{
		t_loopInThisThread = this;
	}
}

EventLoop::~EventLoop()
{
	LOG_DEBUG<<"EventLoop "<<this<<" of thread "<<_threadId
		     <<" destructs in thread "<<CurrentThread::tid();
	t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
	assert(!_looping);
	assertInLoopThread();
	_looping = true;
	_quit = false;
	LOG_TRACE <<"EventLoop "<<this<<" start looping";
	::sleep(5);//::poll(NULL,0,5*1000);
	LOG_TRACE<<"EventLoop "<<this<<" stop looping";
	_looping = false;

}

void EventLoop::quit()
{
	_quit = true;
}

void EventLoop::abortNotInLoopThread()
{
	LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop "<<this
			  << " was created in _threadId = "<<_threadId
			  << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::updateChannel(Channel* channel)
{
	//fixme
	//assert(channel->ownerLoop() == this);
	//poller_->updateChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
//	assert(channel->ownerLoop());
	//fixme
	//return poller_->hanChannel(channel);
	return true;
}

