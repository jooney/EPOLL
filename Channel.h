/*
 * Channel.h
 *
 *  Created on: 2017Äê9ÔÂ24ÈÕ
 *      Author: jjz
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_
#include "Timestamp.h"
#include <functional>
class EventLoop;

class Channel
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;
	//typedef std::function<void()>
	Channel(EventLoop* loop, int fd);
	~Channel();

	void handleEvent(Timestamp receiveTime);
	void setReadCallback(const ReadEventCallback& cb)
	{
		_readCallback = cb;
	}
private:
	void update();
	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;
	EventLoop* _loop;
	const int _fd;
	int _events;
	int _revents;  //it's the received event types of epoll or poll
	int _index;    //used by poller
	ReadEventCallback _readCallback;
	EventCallback  _writeCallback;
	EventCallback  _closeCallback;
	EventCallback  _errorCallback;
};



#endif /* CHANNEL_H_ */
