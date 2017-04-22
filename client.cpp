#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
using namespace std;
#define PORT 12345
#define IP "127.0.0.1"
int main()
{
	int sockfd;
	sockaddr_in servaddr;
	sockfd = ::socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET,IP,&servaddr.sin_addr);
	connect(sockfd,(sockaddr*)&servaddr,sizeof(servaddr));
	return 0;
}
