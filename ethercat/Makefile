CXX=g++
LD=g++
CXXFLAGS=-fPIC
LDFLAGS=-shared -fPIC

OBJS=link.o datagram.o packet.o slave.o

all: libethercat.so

link.o: link.cpp link.h
datagram.o: datagram.cpp datagram.h
packet.o: packet.cpp packet.h
slave.o: slave.cpp slave.h

libethercat.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libethercat.so $^

.PHONY: clean

clean:
	rm -rf *~ $(OBJS) libethercat.so