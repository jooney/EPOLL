#ifndef __SERVER_H__
#define __SERVER_H__

#include <muduo/net/TcpServer.h>
class EchoServer
{
	public:
		EchoServer(muduo::net::EventLoop* loop,
				const muduo::net::InetAddress& listenAddr);
	private:
		muduo::net::EventLoop* loop_;
		muduo::net::TcpServer server_;
};







#endif
