CXX=g++
LD=g++
CXXFLAGS=-fPIC
LDFLAGS=-shared -fPIC

OBJS=connection.o datagram.o

all: libethercat.so

connection.o: connection.cpp connection.h
datagram.o: datagram.cpp datagram.h

libethercat.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libethercat.so $^

.PHONY: clean

clean:
	rm -rf *~ $(OBJS) libethercat.so
