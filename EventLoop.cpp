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
EventLoop::EventLoop()
	:_looping(false),
	 _quit(false)
{
	LOG_FUNC();
}

EventLoop::~EventLoop()
{
	LOG_FUNC();
}

void EventLoop::loop()
{
	_looping = true;
	::sleep(5);//::poll(NULL,0,5*1000);
	std::cout<<"EventLoop this loop stopped"<<std::endl;
	_looping = false;

}

void EventLoop::updateChannel(Channel* channel)
{
	//fixme
	assert(channel->ownerLoop() == this);
	//poller_->updateChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
	assert(channel->ownerLoop());
	//fixme
	//return poller_->hanChannel(channel);
	return true;
}

