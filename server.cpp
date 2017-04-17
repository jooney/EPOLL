#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
using namespace std;


#define USER_LIMIT 5
#define IP  "127.0.0.1"
#define PORT 12345
#define LINE 1024
#define BUFFER_SIZE 1024
const char* msg = "I recv your msg: ";
struct client_data
{
	sockaddr_in cliaddr;
	char* write_buf;
	char buf[BUFFER_SIZE];
};

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock,F_GETFL);
	opts |= O_NONBLOCK;
	fcntl(sock,F_SETFL,opts);
}

int main()
{

	struct sockaddr_in servaddr, cliaddr;
	struct epoll_event ev, events[1024];
	int connfd;
	socklen_t clilen;
	bzero(&servaddr,sizeof(servaddr));
	bzero(&cliaddr,sizeof(cliaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET,IP,&servaddr.sin_addr);

	int listenfd = ::socket(PF_INET,SOCK_STREAM,0);
	assert(listenfd >= 0);
	int ret, epfd, nfds;
	ret = ::bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	assert(ret != -1);
	ret = ::listen(listenfd,5);
	assert(ret != -1);
	epfd = epoll_create(256);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN;
	client_data* users = new client_data[USER_LIMIT];
	int user_counter = 0;
	//initialize user
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	char line[LINE];
	int nread = 0, sockfd;
	for (;;)	
	{
		nfds = epoll_wait(epfd,events,1024,-1);
		for (int i = 0;i<nfds;i++)
		{
			if (events[i].data.fd == listenfd)
			{
				connfd = ::accept(listenfd,(sockaddr*)&cliaddr,&clilen);	
				if (connfd < 0)
				{
					perror("connfd<0");
					exit(1);
				}
				setnonblocking(connfd);
				const char* str = inet_ntoa(cliaddr.sin_addr);
				cout<<"connect from "<<str<<endl;
				ev.data.fd = connfd;
				ev.events = EPOLLIN ;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
			}
			else if (events[i].events & EPOLLIN)
			{
				if ( (sockfd = events[i].data.fd) <0) continue;
				bzero(&line,sizeof(line));
				if ( (nread = ::read(sockfd,line,LINE))<0)
				{
					if (errno == ECONNRESET)
					{
						close(sockfd);
						events[i].data.fd = -1;
					}
					else
						cout<<"readline error"<<endl;
				}
				else if(nread==0)
				{
					close(sockfd);
					events[i].data.fd = -1;
				}
				cout<<"recv info from client: "<<line<<endl;;
				ev.data.fd = sockfd;
				ev.events = EPOLLOUT;
				epoll_ctl(epfd,EPOLL_CTL_MOD, sockfd, &ev);
			}
			else if (events[i].events & EPOLLOUT)
			{
			//	bzero(&line,sizeof(line));
					
				bcopy(msg,line,strlen(msg));
				ret = ::write(events[i].data.fd,line,strlen(line));
				if (ret < 0)
				{
					cout<<"server send msg to client error"<<endl;
					return -1;
				}
				ev.data.fd = events[i].data.fd;
				ev.events = EPOLLIN;
				epoll_ctl(epfd,EPOLL_CTL_MOD,events[i].data.fd,&ev);
			}
		}
	}
	return 0;
}

