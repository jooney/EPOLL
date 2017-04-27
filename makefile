TARGET =  server 
MUDUO = /home/jjz/build/debug-install/lib
LDFLAGS = -pthread -lrt -llog4cplus   $(MUDUO)/libmuduo_net.a  $(MUDUO)/libmuduo_base.a    
INCLUDE = /home/jjz/build/debug-install/include 
CFLAGS = -g 

#server: main.o server.o
#	$(CXX) main.o server.o $(LDFLAGS) -o server
#
#main.o: main.cpp
#	$(CXX) -c main.cpp -I$(INCLUDE) -o main.o
#
#server.o: server.cpp
#	$(CXX) -I$(INCLUDE) -c server.cpp -o server.o



%.o:%.cpp
	$(CXX) -I$(INCLUDE)  -c -g $< 


SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))

$(TARGET):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)



clean:
	-rm server
	-rm *.o
