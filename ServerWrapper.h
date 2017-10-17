#ifndef __SERVERWRAPPER_H__
#define __SERVERWRAPPER_H__
#include  "TcpServer.h"
#include  "Log.h"
class ServerWrapper
{
	public:
		ServerWrapper(EventLoop* loop,
				      const InetAddress& listenAddr,std::string name, ZQ::common::Log&);
		void start();
	private:
		void onConnection(const TcpConnectionPtr& conn);
		void onMessage(const TcpConnectionPtr& conn,
					   Buffer* buf, 
					   Timestamp time);
		void onWriteComplete(const TcpConnectionPtr& conn);
		TcpServer _server;
};



#endif
