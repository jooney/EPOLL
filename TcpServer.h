#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "base/Types.h"
#include "TcpConnection.h"
#include <functional>

class Acceptor;
class EventLoop;

class TcpServer
{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallback;
};

#endif
