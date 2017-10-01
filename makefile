TARGET = TcpServer 
MUDUO = /home/jjz/muduotest/
LDFLAGS = -pthread -lrt $(MUDUO)/base/libmuduo_base.a  # $(MUDUO)/libmuduo_net.a  $(MUDUO)/libmuduo_base.a    
INCLUDE = -I. -I$(MUDUO)base#-I$(MUDUO)poller 
CFLAGS = -g -std=c++11
OBJS = Acceptor.o Buffer.o Channel.o EventLoop.o EventLoopThread.o EventLoopThreadPool.o InetAddress.o \
	   Poller.o Socket.o SocketsOps.o TcpConnection.o TcpServer.o main.o DefaultPoller.o EPollPoller.o
VPATH = .:poller
$(TARGET):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJS):%.o:%.cpp
	$(CXX) -c $(INCLUDE) $(CFLAGS) $< -o $@
clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJS)
