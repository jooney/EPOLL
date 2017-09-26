/*
 * Thread.h
 *
 *  Created on: 2017��9��22��
 *      Author: jjz
 */

#ifndef THREAD_H_
#define THREAD_H_


#include <functional>
#include <memory>
#include <pthread.h>
class Thread
{
public:
	typedef std::function<void()> ThreadFunc;
	Thread(const ThreadFunc&,const std::string& name);
	~Thread();
	void start();
	int join();
	bool started() const {return _started;}

	const std::string& name() const {return _name;}
private:
	bool         _started;
	bool         _joined;
	pthread_t    _tid;              //thread id
	ThreadFunc   _func;
	std::string  _name;

};




#endif /* THREAD_H_ */
