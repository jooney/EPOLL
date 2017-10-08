#include "TcpConnection.h"
#include "Logging.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"
#include "Channel.h"
using namespace std::placeholders;

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
	LOG_INFO  << conn->localAddress().toIpPort() <<" -> "
		      << conn->peerAddress().toIpPort() << " is "
			  << (conn->connected() ? "UP":"DOWN");
	//if (conn->connected())
	//{
	//	conn->send(Timestamp::now().toFormattedString() + "\n");
	//	conn->shutdown();
	//}

}

void defaultMessageCallback(const TcpConnectionPtr& conn,
							Buffer* buf,
							Timestamp time)
{
	string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "<< msg <<" bytes received at " << time.toString();
	conn->send(msg);
}

void defaultWriteCompleteCallback(const TcpConnectionPtr& conn)

{
	LOG_INFO << "write complete";
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
	_peerAddr(peerAddr),
	_closeCallback(NULL)
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

void TcpConnection::shutdown()
{
	_loop->assertInLoopThread();
	if (_state == kConnected)
	{
		setState(kDisconnecting);
		_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
	}
	
}

void TcpConnection::shutdownInLoop()
{
	_loop->assertInLoopThread();
	if (!_channel->isWriting())
	{
		//we are not writing
		_socket->shutdownWrite();
	}
}

void TcpConnection::send(const void* data, int len)
{
	send(StringPiece(static_cast<const char*>(data),len));
}

void TcpConnection::send(const StringPiece& message)
{
	if (_state == kConnected)
	{
		if (_loop->isInLoopThread())
		{
			sendInLoop(message);
		}
		else
		{
			_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this,message.as_string()));
		}
	}
}

void TcpConnection::send(Buffer* buf)
{
	if (_state == kConnected)
	{
		if (_loop->isInLoopThread())
		{
			sendInLoopEX(buf->peek(), buf->readableBytes());
			buf->retrieveAll();
		}
		else
		{
			_loop->runInLoop(std::bind(&TcpConnection::sendInLoop,
						     this, buf->retrieveAllAsString()));
		}
	}
}

void TcpConnection::sendInLoop(const StringPiece& message)
{
	sendInLoopEX(message.data(),message.size());
}

void TcpConnection::sendInLoopEX(const void* data,size_t len)
{
	LOG_INFO << "TcpConnection::sendInLoopEx"; 
	_loop->assertInLoopThread();
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;
	if (_state == kDisconnected)
	{
		LOG_INFO << "disconnected, give up writing";
		return; 
	}
	if (!_channel->isWriting() && _outputBuffer.readableBytes())
	{
		nwrote = sockets::write(_channel->fd(),data,len);
		LOG_INFO << "TcpConnection::sendInLoopEx: "<< nwrote << "bytes"; 
		if (nwrote >= 0)
		{
			remaining = len - nwrote;
			if (remaining == 0 && _writecompleteCB)
			{
				_loop->queueInLoop(std::bind(_writecompleteCB,shared_from_this()));
			}
		}
		else
		{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
			{
				LOG_SYSERR << "TcpConnection::sendInLoop";
				if (errno == EPIPE || errno == ECONNRESET)
				{
					faultError = true;
				}
			}
		}
	}
	assert(remaining <= len);
	if (!faultError && remaining > 0)
	{
		LOG_INFO << "TcpConnection::sendInLoopEX remaining: " << remaining;
//		size_t oldLen = _outputBuffer.readableBytes();
		//fixme
		_outputBuffer.append(static_cast<const char*>(data)+nwrote,remaining);
		if (!_channel->isWriting())
		{
			_channel->enableWriting();
		}
	}
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
	_loop->assertInLoopThread();
	if (_channel->isWriting())
	{
		ssize_t n = sockets::write(_channel->fd(),
								  _outputBuffer.peek(),
								  _outputBuffer.readableBytes());
		if (n > 0)
		{
			_outputBuffer.retrieve(n);
			if (_outputBuffer.readableBytes() == 0)
			{
				_channel->disableWriting();
				if (_writecompleteCB)
				{
					_loop->queueInLoop(std::bind(_writecompleteCB,shared_from_this()));
				}
				if (_state == kDisconnecting)
				{
					shutdownInLoop();
				}
			}
		}
		else
		{
			LOG_SYSERR << "TcpConnection::handleWrite";
		}
	}
}

void TcpConnection::handleClose()
{
	_loop->assertInLoopThread();
	LOG_INFO << "TcpConnection::handleClose() fd = " <<_channel->fd() << " state = " << stateToString();
	assert( _state == kConnected || _state == kDisconnecting);
	setState(kDisconnected);
	_channel->disableAll();
	TcpConnectionPtr guardThis(shared_from_this());
	_connectionCallback(guardThis);   // ?????
	_closeCallback(guardThis); // call TcpServer::removeConnection
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(_channel->fd());
	LOG_ERROR << "TcpConnection::handleError [" << _name
		      << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::connectDestroyed()
{
	_loop->assertInLoopThread();
	if (_state == kConnected)
	{
		setState(kDisconnected);
		_channel->disableAll();
		_connectionCallback(shared_from_this());   //?????
	}
	_channel->remove();
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
