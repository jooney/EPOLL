/*
 * EventLoop.h
 *
 *  Created on: 2017Äê9ÔÂ23ÈÕ
 *      Author: jjz
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "Thread.h"
class Channel;
class EventLoop
{
public:
	EventLoop();
	~EventLoop();
	void loop();
	void updateChannel(Channel* channel);
	bool hasChannel(Channel* channel);
	//void assertInLoopThread();
	//static EventLoop* getEventLoopOfCurrentThread();

private:
	//bool isInLoopThread();
	//void abortNotInLoopThread();
	bool _looping;
	bool _quit;
	//static EventLoop* t_loopInThisThread;
	//const pthread_t _threadId;

};



#endif /* EVENTLOOP_H_ */
