/*
 * EventLoop.cpp
 *
 *  Created on: 2017Äê9ÔÂ23ÈÕ
 *      Author: jjz
 */
#include "Logging.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "iostream"
#include <unistd.h>
#include <assert.h>

#define LOG_FUNC() printf("func here %s\n",__FUNCTION__)

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	:_looping(false),
	 _quit(false),
	 _eventHandling(false),
	 _threadId(CurrentThread::tid()),
	 _currentActiveChannel(NULL),
	 _poller(Poller::newDefaultPoller(this))
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

//runs callback immediately in the loop threads
//it wakes up the loop, and run cb
void EventLoop::runInLoop(const Functor& cb)
{
	if (isInLoopThread())
	{
		cb();
	}
}

void EventLoop::loop()
{
	assert(!_looping);
	assertInLoopThread();
	_looping = true;
	_quit = false;
	LOG_TRACE <<"EventLoop "<<this<<" start looping";
	while (!_quit)
	{
		_activeChannels.clear();
		_pollReturnTime = _poller->poll(kPollTimeMs,&_activeChannels);
		if (Logger::logLevel() <= Logger::TRACE)
		{
			printActiveChannels();
		}
		_eventHandling = true;
		for (ChannelList::iterator it = _activeChannels.begin();
			it != _activeChannels.end(); ++it)
		{
			_currentActiveChannel = *it;
			_currentActiveChannel->handleEvent(_pollReturnTime);
		}
		_currentActiveChannel = NULL;
		_eventHandling = false;

	}
	//::sleep(5);//::poll(NULL,0,5*1000);
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
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	_poller->updateChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
//	assert(channel->ownerLoop());
	//fixme
	//return poller_->hanChannel(channel);
	return true;
}

void EventLoop::printActiveChannels() const
{
	for (ChannelList::const_iterator it = _activeChannels.begin();
		it != _activeChannels.end();++it)
	{
		const Channel* ch = *it;
		LOG_TRACE << "(" << ch->reventsToString() << ")";
	}
}

