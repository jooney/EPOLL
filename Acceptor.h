#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <boost/noncopyable.hpp>
#include <functional>
#include "Socket.h"
class InetAddress;
class EventLoop;
class Acceptor : boost::noncopyable
{
	public:
		typedef std::function<void(int sockfd,const InetAddress&)>	NewConnectionCallback;
		Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
		~Acceptor();
		void setNewConnectionCallback(const NewConnectionCallback& cb)
		{
			newConnectionCB = cb;
		}
		bool listening() const {return _listenning;}
		void listen();
	private:
		EventLoop* _loop;
		Socket _acceptSocket;
		NewConnectionCallback newConnectionCB;
		bool _listenning;
		int _idleFd;
};
#endif
