#include "TcpServer.h"
#include "Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "SocketsOps.h"
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop,
					 const InetAddress& listenAddr,
					 const std::string nameArg,
					 Option option)
	:_loop(loop),
	_ipPort(listenAddr.toIpPort()),
	_name(nameArg),
	_threadInitCallback(NULL),
	_nextConnId(1),
	_connectionCallback(defaultConnectionCallback),
	_messageCallback(defaultMessageCallback),
	_acceptor(new Acceptor(loop,listenAddr,option==kReusePort)),
	_threadPool(new EventLoopThreadPool(loop,_name))
{
	_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,_1,_2));
	LOG_INFO<<"TcpServer::TcpServer";
}

TcpServer::~TcpServer()
{
	_loop->assertInLoopThread();
	LOG_INFO<<"TcpServer::~TcpServer["<<_name<<"] destructing";
}

void TcpServer::start()
{
	LOG_INFO<<"TcpServer::start()";
	if (_started.getAndSet(1) == 0)
	{
		_threadInitCallback = NULL;
		_threadPool->start(_threadInitCallback);
		assert(!_acceptor->listening());
		_loop->runInLoop(std::bind(&Acceptor::listen,_acceptor.get()));	
	}
}

//TcpServer.cpp _acceptor->setNewConnectCallback();
void TcpServer::newConnection(int sockfd,const InetAddress &peerAddr)
{
	_loop->assertInLoopThread();
	EventLoop* ioLoop = _threadPool->getNextLoop();
	char buf[64];
	snprintf(buf,sizeof(buf),"-%s#%d",_ipPort.c_str(),_nextConnId);
	++_nextConnId;
	std::string connName = _name + buf;
	LOG_INFO << "TcpServer::newConnection [" << _name << "] - new connection [" << connName << "] from " << peerAddr.toIpPort();
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(ioLoop,
											connName,
											sockfd,
											localAddr,
											peerAddr));
	_connections[connName] = conn;
	conn->setConnectionCallback(_connectionCallback);
	conn->setMessageCallback(_messageCallback);
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));


}
