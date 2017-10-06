#include "TcpConnection.h"
#include "Logging.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
	LOG_INFO  << conn->localAddress().toIpPort() <<" -> "
		      << conn->peerAddress().toIpPort() << " is "
			  << (conn->connected() ? "UP":"DOWN");

}

void defaultMessageCallback(const TcpConnectionPtr& conn,
							Buffer* buf,
							Timestamp time)
{
	string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << "discares " << msg.size()
		     << " bytes received at " << time.toString();
}
TcpConnection::TcpConnection(EventLoop* loop,
							const std::string nameArg,
							int sockfd,
							const InetAddress& localAddr,
							const InetAddress& peerAddr)
	:_channel(new Channel(loop,sockfd)),
	_loop(loop),
	_name(nameArg),
	_state(kConnecting),
	_reading(true),
	_socket(new Socket(sockfd)),
	_localAddr(localAddr),
	_peerAddr(peerAddr)
{
	LOG_DEBUG << "TcpConnection::TcpConnection "<< this <<"loop: "<< loop;
	_channel->setReadCallback(std::bind(&TcpConnection::handleRead,this,std::placeholders::_1));
	_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
	_channel->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
	_channel->setErrorCallback(std::bind(&TcpConnection::handleError,this));
	LOG_DEBUG << "TcpConnection::ctor[" << _name<<"] at "<< this << " fd="<< sockfd << "loop: "<< loop;
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
	_channel->tie(shared_from_this());
	_channel->enableReading();
	LOG_DEBUG << "TcpConnection::connectEstablished";
	_connectionCallback(shared_from_this());//call callback.h defaultConnectionCallback(TcpConnectionPtr)
}
void TcpConnection::handleRead(Timestamp receiveTime)
{
	_loop->assertInLoopThread();
	int savedErrno = 0;
	ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedErrno);
	if (n>0)
	{
		_messageCallback(shared_from_this(), &_inputBuffer, receiveTime);	
	}
	else if (n == 0)
	{
		handleClose();
	}
	else
	{
		errno = savedErrno;
		LOG_SYSERR << "TcpConnection::handleRead";
		handleError();
	}

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
