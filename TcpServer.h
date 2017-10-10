#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Log.h"
#include <string>
#include "Atomic.h"
#include "Types.h"
#include "TcpConnection.h"
#include <functional>
#include <boost/noncopyable.hpp>
#include <map>

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
				  const std::string nameArg,
				  ZQ::common::Log& log,
				  Option option = kNoReusePort);
		~TcpServer();

		const string& ipPort()const {return _ipPort;}; 
		const std::string name() const {return _name;}
		EventLoop* getLoop()const {return _loop;}
		void setThreadInitCallback(const ThreadInitCallback& cb){_threadInitCallback = cb;}
		std::shared_ptr<EventLoopThreadPool> threadPool(){return _threadPool;}
		void start();
		//for Wrapper call
		void setConnectionCallback(const ConnectionCallback& cb)
		{ _connectionCallback = cb;}
		void setMessageCallback(const MessageCallback& cb)
		{ _messageCallback = cb;}
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ _writecompleteCB = cb;}
	private:
		void newConnection(int sockfd,const InetAddress& peerAddr);
		void removeConnection(const TcpConnectionPtr& conn);
		void removeConnectionInLoop(const TcpConnectionPtr& conn);
		EventLoop* _loop;    //the acceptor loop
		const string _ipPort;
		boost::scoped_ptr<Acceptor> _acceptor;
		ConnectionCallback _connectionCallback;
		MessageCallback    _messageCallback;
		WriteCompleteCallback _writecompleteCB;
		ThreadInitCallback   _threadInitCallback;
		//const string  _name;
		const std::string _name;
		std::shared_ptr<EventLoopThreadPool> _threadPool;
		AtomicInt32  _started;
		int   _nextConnId;
		typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;
		ConnectionMap _connections;
		ZQ::common::Log&  _log;

};

#endif
