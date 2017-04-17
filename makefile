TARGET = test 
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
	-rm test
	-rm *.o
