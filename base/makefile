TARGET = libmuduo_base.a
LDFLAGS = -pthread -lrt     
INCLUDE = -I.
CFLAGS = -g 

%.o:%.cc%.h
	$(CXX) $(INCLUDE)  -c -g -std=c++11 $< 


SOURCES = $(wildcard *.cc)
OBJS = $(patsubst %.cc,%.o,$(SOURCES))

$(TARGET):$(OBJS)
	ar cr $(TARGET) $(OBJS)



clean:
	-rm libmuduo_base.a
	-rm *.o
