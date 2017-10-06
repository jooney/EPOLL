#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__
#include "StringPiece.h"
#include "Types.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"
#include <memory>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
struct tcp_info;

class Channel;
class EventLoop;
class Socket;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> 
{
	public:
		TcpConnection(EventLoop* loop,
				      const std::string name,
					  int sockfd,
					  const InetAddress& localAddr,
					  const InetAddress& peerAddr);
		~TcpConnection();

		EventLoop* getLoop() const {return _loop; }
		const std::string name() const {return _name;}
		const InetAddress& localAddress() const {return _localAddr;}
		const InetAddress& peerAddress() const {return _peerAddr;}
		bool  connected() const {return _state == kConnected;}
		bool  disconnected() const {return _state == kDisconnected;}

		bool getTcpInfo(struct tcp_info*)const;
		string getTcpInfoString() const;
		void connectEstablished();
		void setConnectionCallback(const ConnectionCallback& cb)
		{_connectionCallback = cb;}
		void setMessageCallback(const MessageCallback& cb)
		{ _messageCallback = cb;}


	private:
		enum StateE {kDisconnected, kConnecting, kConnected, kDisconnecting};
		void handleRead(Timestamp receiveTime);
		void handleWrite();
		void handleClose();
		void handleError();
		void setState(StateE s) {_state = s;}
		const char* stateToString() const;

		EventLoop* _loop;
		StateE _state;
		const std::string _name;
		bool  _reading;
		boost::scoped_ptr<Socket>  _socket;
		boost::scoped_ptr<Channel> _channel;
		const InetAddress _localAddr;
		const InetAddress _peerAddr;
		ConnectionCallback   _connectionCallback;
		MessageCallback      _messageCallback;
		Buffer   _inputBuffer;
};
#endif
