#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "muduo/base/Atomic.h"
#include "base/Types.h"
#include "TcpConnection.h"
#include <functional>

class Acceptor;
class EventLoop;

class TcpServer
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

		const string& ipPort(); 
	private:
		EventLoop* _loop;    //the acceptor loop
		const string _ipPort;
		const string _name;

};

#endif
