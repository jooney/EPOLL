TARGET =  EventLoop 
MUDUO = /home/jjz/git/build/debug-install#lib/
LDFLAGS = -pthread -lrt $(MUDUO)/lib/libmuduo_base.a  # $(MUDUO)/libmuduo_net.a  $(MUDUO)/libmuduo_base.a    
INCLUDE = -I.  -IMUDUO/include/ 
CFLAGS = -g 

#server: main.o server.o
#	$(CXX) main.o server.o $(LDFLAGS) -o server
#
#main.o: main.cpp
#	$(CXX) -c main.cpp -I$(INCLUDE) -o main.o
#
#server.o: server.cpp
#	$(CXX) $(INCLUDE) -c server.cpp -o server.o



%.o:%.cpp %.h
	$(CXX) $(INCLUDE)  -c -g -std=c++11 $< 


SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))

$(TARGET):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)



clean:
	-rm EventLoop
	-rm *.o
