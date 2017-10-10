#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <boost/noncopyable.hpp>
#include <functional>
#include "Channel.h"
#include "Socket.h"
#include "Log.h"
class InetAddress;
class EventLoop;

class Acceptor : boost::noncopyable
{
	public:
		typedef std::function<void(int sockfd,const InetAddress&)>	NewConnectionCallback;
		Acceptor(EventLoop* loop, const InetAddress& listenAddr, ZQ::common::Log& log,bool reuseport);
		~Acceptor();
		void setNewConnectionCallback(const NewConnectionCallback& cb)
		{
			_newConnectionCB = cb;
		}
		bool listening() const {return _listenning;}
		void listen();
	private:
		void handleRead();
		EventLoop* _loop;
		Socket _acceptSocket;
		NewConnectionCallback _newConnectionCB;
		Channel   _acceptChannel;
		bool _listenning;
		int _idleFd;
		ZQ::common::Log&   _log;
};
#endif
