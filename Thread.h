/*
 * Thread.h
 *
 *  Created on: 2017Äê9ÔÂ22ÈÕ
 *      Author: jjz
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <functional>
#include <memory>
class Thread
{
public:
	typedef std::function<void()> ThreadFunc;
	Thread(const ThreadFunc&, const std::string& name);
	~Thread();
	void start();
	int join();
	bool started() const {return _started;}
	//pid_t pid()const{return *_pid;}
	const std::string& name() const {return _name;}
private:
	bool         _started;
	bool         _joined;
	pthread_t    _tid;              //thread id
	//std::shared_ptr<pid_t> _pid;    //process id
	ThreadFunc   _func;
	std::string  _name;

};




#endif /* THREAD_H_ */
