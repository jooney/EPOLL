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
	_writecompleteCB(defaultWriteCompleteCallback),
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
	ConnectionMap::iterator it;
	for (it = _connections.begin(); it != _connections.end(); ++it)
	{
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
		conn.reset();
	}
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
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,_1));
	conn->setWriteCompleteCallback(_writecompleteCB);
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	_loop->assertInLoopThread();
	LOG_INFO << "TcpServer::removeConnectionInLoop [" << _name
		     << "] - connection " << conn->name();
	size_t n = _connections.erase(conn->name());
	(void)n ;  //?????
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));

}
