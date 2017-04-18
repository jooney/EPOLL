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
#include <pthread.h>
#include <iostream>
using namespace std;


#define USER_LIMIT 5
#define IP  "127.0.0.1"
#define PORT 12345
#define LINE 1024
#define BUFFER_SIZE 1024
const char* msg = "I recv your msg: ";

struct task
{
	int fd;
	struct task* next;
};

struct user_data
{
	int fd;
	unsigned int n_size;
	char line[LINE];
};

void* readtask(void* args);

void* writetask(void* args);

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
	pthread_t tid1, tid2;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	struct task* next_task = NULL, *readhead = NULL,*readtail = NULL, *writehead = NULL;
	struct user_data* rdata = NULL;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
	int connfd;
	socklen_t clilen;
	bzero(&servaddr,sizeof(servaddr));
	bzero(&cliaddr,sizeof(cliaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET,IP,&servaddr.sin_addr);

	int listenfd = ::socket(PF_INET,SOCK_STREAM,0);
	assert(listenfd >= 0);
	int ret, epfd, nfds,option =1;
	if ( setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option))<0)
	{
		perror("setsockopt reuseaddr error: ");
	}
	ret = ::bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	assert(ret != -1);
	ret = ::listen(listenfd,5);
	assert(ret != -1);
	epfd = epoll_create(256);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	char line[LINE],climsg[LINE];
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
				ev.events = EPOLLIN;
				ev.events |= EPOLLET; //et mode
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
			}
			else if (events[i].events & EPOLLIN)
			{
				if ( (sockfd = events[i].data.fd) <0) continue;
				bzero(&climsg,sizeof(climsg));
				if ( (nread = ::read(sockfd,climsg,5))<0) //test epoll LT mode
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
					continue;
				}
				cout<<"recv info from client: "<<climsg<<endl;;
				//ev.data.fd = sockfd;
				//ev.events = EPOLLOUT;
				//epoll_ctl(epfd,EPOLL_CTL_MOD, sockfd, &ev);
			}
			else if (events[i].events & EPOLLOUT)
			{
				bzero(&line,sizeof(line));
					
				bcopy(msg,line,strlen(msg));
				strcat(line,climsg);
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
	::close(listenfd);
	return 0;
}

void* readtask(void* args)
{
}


void* writetask(void* args)
{
}
