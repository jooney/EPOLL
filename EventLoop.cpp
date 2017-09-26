/*
 * EventLoop.cpp
 *
 *  Created on: 2017Äê9ÔÂ23ÈÕ
 *      Author: jjz
 */
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
	 _threadId(getpid()) //???CurrentTHread::tid()
{
	LOG_FUNC();
	printf("EventLoop created [%p] in thread[%d]\n",this,_threadId);
	if (t_loopInThisThread)
	{
		printf("Anothrer EventLoop [%p] exists in this thread[%d]\n",t_loopInThisThread,_threadId);
	}
	else
	{
		t_loopInThisThread = this;
	}
}

EventLoop::~EventLoop()
{
	LOG_FUNC();
	printf("EventLoop[%p] of thread[%d] destructs in thread\n",this,_threadId);
	t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
	_looping = true;
	::sleep(5);//::poll(NULL,0,5*1000);
	std::cout<<"EventLoop this loop stopped"<<std::endl;
	_looping = false;

}

void EventLoop::quit()
{
	_quit = true;
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

