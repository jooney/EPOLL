#include "Acceptor.h"
#include "Logging.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
//using namespace sockets;
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
	:_loop(loop),
	_listenning(false),
	_acceptSocket(sockets::createNonblockingOrDie(listenAddr.family())) 
{
	LOG_INFO<<"Acceptor::Acceptor()";
}

Acceptor::~Acceptor()
{
	LOG_INFO<<"Acceptor::~Acceptor()";
}

void Acceptor::listen()
{
	_loop->assertInLoopThread();
	_listenning = true;
	_acceptSocket.listen();


}
