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
#include "server.h"
using namespace std;


#define USER_LIMIT 5
#define IP  "127.0.0.1"
#define PORT 12345
#define LINE 1024
#define BUFFER_SIZE 1024
const char* msg = "I recv your msg: ";

int main()
{

	return 0;
}
