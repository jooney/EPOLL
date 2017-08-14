#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include "server.h"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
using namespace std;

#define IP  "127.0.0.1"
#define PORT 12345
const char* g_file = "testfile";
void onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO<<"FileServer- "<<conn->peerAddress().toIpPort()<<" -> "
		<<conn->localAddress().toIpPort()<<" is "
		<<(conn->connected()?"UP":"DOWN");
	if (conn->connected())
	{
		LOG_INFO<<"FileServer - Sending file "<<g_file<<" to "
			<<conn->peerAddress().toIpPort();
		FILE* fp = ::fopen(g_file,"rb");
		if (fp)
		{
			conn->setContext(fp);
			char buf[10];
			size_t nread = ::fread(buf,1,sizeof(buf),fp);
			conn->send(buf,static_cast<int>(nread));
			LOG_INFO<<"file has been sent";
		}
		else
		{
			LOG_INFO<<"fp not valid";
			conn->shutdown();
			LOG_INFO<<"FileServer - no such file";
		}
	}
}
int main(int argc, char* argv[])
{
	LOG_INFO << "pid = " << getpid();
	muduo::net::EventLoop loop;
	muduo::net::InetAddress listenAddr(12345);
	muduo::net::TcpServer server(&loop,listenAddr,"FileServer");
	server.setConnectionCallback(onConnection);
	server.start();
	loop.loop();
	return 0;
}
