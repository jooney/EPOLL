#include <iostream>
#include "ServerWrapper.h"
#include "EventLoop.h"
#include "FileLog.h"

using namespace std;
int main()
{
	std::string name = "TcpServer";
	ZQ::common::FileLog  serverlog("server.log", 7);
	EventLoop loop(serverlog);
	InetAddress listenAddr(2009);
	ServerWrapper wrapper(&loop,listenAddr,name,serverlog);
	wrapper.start();
	loop.loop();
	return 0;
}
