/*
 * Channel.cpp
 *
 *  Created on: 2017Äê9ÔÂ24ÈÕ
 *      Author: jjz
 */
#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>
#include <assert.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
	:_loop(loop),
	_fd(fd),
	_events(0),
	_revents(0),
	_index(-1),
	_eventHandling(false),
	_addedToLoop(false),
	_tied(false)
{
}

Channel::~Channel()
{
	assert(!_eventHandling);
	assert(!_addedToLoop);
	assert(!_loop->hasChannel(this));
}
void Channel::update()
{
	_addedToLoop = true;
	_loop->updateChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
}

