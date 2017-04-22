TARGET: server client 
LDFLAGS = -lpthread -lrt -llog4cplus
INCLUDE = . 
CFLAGS = -g 

%.o:%.cpp
	$(CXX) -I$(INCLUDE) -c -g $<  

SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))
$(TARGET):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)



clean:
	-rm server client
	-rm *.o
