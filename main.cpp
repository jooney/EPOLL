#include <iostream>
#include "TcpServer.h"
#include "EventLoop.h"

using namespace std;
int main()
{
	std::string name = "TcpServer";
	EventLoop loop;
	InetAddress listenAddr(2009);
	TcpServer server(&loop,listenAddr,name);
	server.start();
	loop.loop();
	return 0;
}
