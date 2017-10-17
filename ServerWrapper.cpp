#include "ServerWrapper.h"
#include <boost/any.hpp>
#include "Logging.h"
using namespace std::placeholders;
const char* g_file = "TcpConnection.cpp";

std::string ReadFile(const char* filename)
{
	std::string content;
	FILE* fp = ::fopen(filename,"rb");
	if (fp)
	{
		const int size = 1024 * 1024;
		char buf[size];
		size_t nRead = 0;
		while ( (nRead = ::fread(buf,1,sizeof(buf),fp)) >0)
		{
			content.append(buf,nRead);
		}
		::fclose(fp);
	}
	return content;
}
ServerWrapper::ServerWrapper(EventLoop* loop,
							const InetAddress& addr, std::string name, ZQ::common::Log& log)
	:_server(loop,addr,name,log)
{
	_server.setConnectionCallback(std::bind(&ServerWrapper::onConnection,this,_1));
	_server.setWriteCompleteCallback(std::bind(&ServerWrapper::onWriteComplete,this,_1));
}

void ServerWrapper::start()
{
	_server.start();
}
void ServerWrapper::onConnection(const TcpConnectionPtr& conn)
{
	LOG_INFO << "onConnection";
	//std::string str = ReadFile("TcpConnection.cpp") ;
	//conn->send(str);
	//conn->shutdown();
	if (conn->connected())
	{
		FILE* fp = ::fopen(g_file,"rb");
		if (fp)
		{
			conn->setContext(fp);
			char buf[128];
			size_t nRead = ::fread(buf,1,sizeof(buf),fp);
			conn->send(buf,nRead);
		}
		else
		{
			conn->shutdown();
		}
	}
	else
	{
		if (!conn->getContext().empty())	
		{
			FILE* fp = boost::any_cast<FILE*>(conn->getContext());
			if (fp) ::fclose(fp);
		}
	}
}

void ServerWrapper::onMessage(const TcpConnectionPtr& conn,
							 Buffer* buf,
							 Timestamp time)
{

}

void ServerWrapper::onWriteComplete(const TcpConnectionPtr& conn)
{
	FILE* fp = boost::any_cast<FILE*>(conn->getContext());
	char buf[128];
	size_t nRead = ::fread(buf,1,sizeof(buf),fp);
	if (nRead >0)
	{
		conn->send(buf,nRead);
	}
	else
	{
		::fclose(fp);
		fp= NULL;
		conn->setContext(fp);
		conn->shutdown();
		LOG_INFO << "FileServer - done";
	}
}
