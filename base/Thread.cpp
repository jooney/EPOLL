/*
 * Thread.cpp
 *
 *  Created on: 2017Äê9ÔÂ22ÈÕ
 *      Author: jjz
 */
#include "Thread.h"
#include <iostream>
#include <assert.h>


namespace detail
{
	struct ThreadData
	{
		typedef Thread::ThreadFunc ThreadFunc;
		ThreadFunc _func;
		std::string _name;
		//std::weak_ptr<pid_t> _wkPid;

		ThreadData(const ThreadFunc& func,
				   const std::string& name)
			:_func(func),
			 _name(name)
		{}
		void runInThread()
		{
			std::cout<<"runInThread() called"<<std::endl;
			_func();
		}
	};

	void* startThread(void* obj)
	{
		//std::cout<<"startThread in ThreadData"<<std::endl;
		ThreadData* data = static_cast<ThreadData*>(obj);
		data->runInThread();
		delete data;
		return NULL;
	}
}

Thread::Thread(const ThreadFunc& func, const std::string& name)
	:_started(false),
	 _joined(false),
	 _func(func),
	_name(name)
{
}


Thread::~Thread()
{
	if (_started && ! _joined)
	{
		pthread_detach(_tid);
	}
	std::cout<<"Thread::~Thread()"<<std::endl;
}

void Thread::start()
{
	assert(!_started);
	_started = true;
	//pthread_detach(_tid);//?????
	detail::ThreadData* data = new detail::ThreadData(_func,_name);
	if (pthread_create(&_tid,NULL,detail::startThread,data))
	{
		_started = false;
		delete data;
		std::cout<<"failed in pthread_create"<<std::endl;
	}
}
int Thread::join()
{
	assert(_started);
	assert(!_joined);
	_joined = true;
	return pthread_join(_tid,NULL);
}

