#include "TcpConnection.h"
#include "Logging.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{

}

void defaultMessageCallback(const TcpConnectionPtr&,
							Buffer* buf,
							Timestamp)
{
}
TcpConnection::TcpConnection(EventLoop* loop,
							const string& nameArg,
							int sockfd,
							const InetAddress& localAddr,
							const InetAddress& peerAddr)
	:_loop(loop),
	_name(nameArg),
	_state(kConnecting),
	_reading(true),
	_socket(new Socket(sockfd)),
	_channel(new Channel(loop,sockfd)),
	_localAddr(localAddr),
	_peerAddr(peerAddr)
{
	_channel->setReadCallback(std::bind(&TcpConnection::handleRead,this,std::placeholders::_1));
	_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
	_channel->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
	_channel->setErrorCallback(std::bind(&TcpConnection::handleError,this));
	LOG_DEBUG << "TcpConnection::ctor[" << _name<<"] at "<< this << " fd="<< sockfd;
}

TcpConnection::~TcpConnection()
{
	LOG_DEBUG << "TcpConnection::dtor[" << _name<<"] at "<< this << "fd=" << _channel->fd()<<" state="<<stateToString();
	assert(_state == kDisconnected);
}

void TcpConnection::connectEstablished()
{
	_loop->assertInLoopThread();
	assert(_state == kConnecting);
	setState(kConnected);	
	_channel->tie(std::shared_ptr<TcpConnection>(this));
	_channel->enableReading();
	_connectionCallback(std::shared_ptr<TcpConnection>(this));//call callback.h defaultConnectionCallback(TcpConnectionPtr)
}
void TcpConnection::handleRead(Timestamp receiveTime)
{


}

void TcpConnection::handleWrite()
{
}

void TcpConnection::handleClose()
{
}

void TcpConnection::handleError()
{
}

const char* TcpConnection::stateToString()const
{
	switch (_state)
	{
		case kDisconnected:
			return "kDisconnected";
		case kConnecting:
			return "kConnecting";
		case kConnected:
			return "kConnected";
		case kDisconnecting:
			return "kDisconnecting";
		default:
			return "unknown state";
	}
}
