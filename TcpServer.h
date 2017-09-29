#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Atomic.h"
#include "Types.h"
#include "TcpConnection.h"
#include <functional>
#include <boost/noncopyable.hpp>

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : boost::noncopyable
{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallback;
		
		enum Option
		{
			kNoReusePort,
			kReusePort
		};
		TcpServer(EventLoop* loop,
				  const InetAddress& listenAddr,
				  const string& nameArg,
				  Option option = kNoReusePort);
		~TcpServer();

		const string& ipPort()const {return _ipPort;}; 
		const string& name() const {return _name;}
		EventLoop* getLoop()const {return _loop;}
		void setThreadInitCallback(const ThreadInitCallback& cb){_threadInitCallback = cb;}
		std::shared_ptr<EventLoopThreadPool> threadPool(){return _threadPool;}
		void start();
	private:
		void newConnection(int sockfd,const InetAddress& peerAddr);
		EventLoop* _loop;    //the acceptor loop
		boost::scoped_ptr<Acceptor> _acceptor;
		std::shared_ptr<EventLoopThreadPool> _threadPool;
		const string _ipPort;
		const string  _name;
		AtomicInt32  _started;
		ThreadInitCallback   _threadInitCallback;

};

#endif
