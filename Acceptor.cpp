#include "Acceptor.h"
#include "Logging.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
	:_loop(loop),
	_listenning(false),
	_acceptSocket(sockets::createNonblockingOrDie(listenAddr.family())),
	_acceptChannel(loop,_acceptSocket.fd())
{
	LOG_INFO<<"Acceptor::Acceptor()";
	_acceptSocket.setReuseAddr(true);
	_acceptSocket.setReusePort(reuseport);
	_acceptSocket.bindAddress(listenAddr);
	_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead,this));

}

Acceptor::~Acceptor()
{
	LOG_INFO<<"Acceptor::~Acceptor()";
}

void Acceptor::listen()
{
	LOG_INFO<<"Acceptor::listen()";
	_loop->assertInLoopThread();
	_listenning = true;
	_acceptSocket.listen();
	_acceptChannel.enableReading();


}

void Acceptor::handleRead()
{
	_loop->assertInLoopThread();
	InetAddress peerAddr;
	int connfd = _acceptSocket.accept(&peerAddr);
	if (connfd >= 0)
	{
		if (_newConnectionCB)
		{
			_newConnectionCB(connfd,peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
	else
	{
		LOG_SYSERR << "in Acceptor::handleRead";
	}

}
