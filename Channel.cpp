/*
 * Channel.cpp
 *
 *  Created on: 2017Äê9ÔÂ24ÈÕ
 *      Author: jjz
 */
#include "Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>
#include <assert.h>
#include <sstream>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd, ZQ::common::Log& log)
	:_loop(loop),
	_fd(fd),
	_events(0),
	_revents(0),
	_index(-1),
	_eventHandling(false),
	_addedToLoop(false),
	_tied(false),
	_log(log)
{
//	LOG_INFO << "Channel  loop: " << _loop;
}

Channel::~Channel()
{
	assert(!_eventHandling);
	assert(!_addedToLoop);
	if (_loop->isInLoopThread())
	{
		assert(!_loop->hasChannel(this));
	}
}

void Channel::tie(const boost::shared_ptr<void>& obj)
{
	_tie = obj;
	_tied = true;
}

void Channel::update()
{
	_addedToLoop = true;
	_loop->updateChannel(this);
}

void Channel::remove()
{
	assert(isNoneEvent());
	_addedToLoop = false;
	_loop->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
	boost::shared_ptr<void> guard;
	if (_tied)
	{
		guard = _tie.lock();
		if (guard)
		{
			handleEventWithGuard(receiveTime);
		}
	}
	else
	{
		handleEventWithGuard(receiveTime);
	}
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
	_eventHandling = true;
	LOG_TRACE << reventsToString();
	if ( (_revents & POLLHUP) && !(_revents & POLLIN))
	{
		if (_closeCallback) 
		{
			_closeCallback();
		}
	}
	if (_revents & POLLNVAL)
	{
	//	LOG_WARN << "fd = "<< _fd<< " Channel::handle_event() POLLNVAL";
	}
	if (_revents & (POLLERR | POLLNVAL))
	{
		if (_errorCallback)
		{
			_errorCallback();
		}
	}
	if (_revents & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (_readCallback)
		{
			_readCallback(receiveTime);
		}
	}
	if (_revents & POLLOUT)
	{
		if (_writeCallback)
		{
			_writeCallback();
		}
	}
	_eventHandling = false;
}

string Channel::eventsToString() const
{
	return eventsToString(_fd,_events);
}

string Channel::reventsToString() const
{
	return eventsToString(_fd,_revents);
}

string Channel::eventsToString(int fd, int ev)
{
	std::ostringstream oss;
	oss << fd << ": ";
	if (ev & POLLIN)
		oss << "IN ";
	if (ev & POLLPRI)
		oss << "PRI ";
	if (ev & POLLOUT)
		oss << "OUT ";
	if (ev & POLLHUP)
		oss << "HUP ";
	if (ev & POLLRDHUP)
		oss << "RDDHUP ";
	if (ev & POLLERR)
		oss << "ERR ";
	if (ev & POLLNVAL)
		oss << "NVAL ";
	return oss.str().c_str();
}
