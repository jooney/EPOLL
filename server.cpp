#include "server.h"

EchoServer::EchoServer(muduo::net::EventLoop*loop,
		const muduo::net::InetAddress& listenAddr)
	:loop_(loop),
	server_(loop,listenAddr,"EchoServer")
{

}
