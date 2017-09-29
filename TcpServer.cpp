#include "TcpServer.h"
#include "Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop,
					 const InetAddress& listenAddr,
					 const string& nameArg,
					 Option option)
	:_loop(loop),
	_ipPort(listenAddr.toIpPort()),
	_name(nameArg),
	_acceptor(new Acceptor(loop,listenAddr,option==kReusePort)),
	_threadPool(new EventLoopThreadPool(loop,_name))
{
	_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,_1,_2));
	LOG_INFO<<"TcpServer::TcpServer";
}

TcpServer::~TcpServer()
{
	_loop->assertInLoopThread();
	LOG_TRACE<<"TcpServer::~TcpServer["<<_name<<"] destructing";
}

void TcpServer::start()
{
	LOG_INFO<<"TcpServer::start()";
	if (_started.getAndSet(1) == 0)
	{
		_threadPool->start(_threadInitCallback);
		assert(!_acceptor->listening());
		_loop->runInLoop(std::bind(&Acceptor::listen,_acceptor.get()));	
	}
}
void TcpServer::newConnection(int sockfd,const InetAddress &peerAddr)
{

}
