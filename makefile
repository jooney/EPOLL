TARGET = TcpServer 
MUDUO = /home/jjz/git/muduotest/
LDFLAGS = -pthread -lrt $(MUDUO)/base/libmuduo_base.a /home/jjz/test/ZQCommon/libZQCommon.so # $(MUDUO)/libmuduo_net.a  $(MUDUO)/libmuduo_base.a    
INCLUDE = -I. -I$(MUDUO)base -I/home/jjz/test/ZQCommon#-I$(MUDUO)poller 
CFLAGS = -g -std=c++11
OBJS = Acceptor.o Buffer.o Channel.o EventLoop.o EventLoopThread.o EventLoopThreadPool.o InetAddress.o \
	   Poller.o Socket.o SocketsOps.o TcpConnection.o TcpServer.o main.o DefaultPoller.o EPollPoller.o ServerWrapper.o
VPATH = .:poller
$(TARGET):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJS):%.o:%.cpp
	$(CXX) -c $(INCLUDE) $(CFLAGS) $< -o $@
clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJS)
