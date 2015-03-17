CXX=g++
LD=g++
CXXFLAGS=-fPIC -std=c++11
LDFLAGS=-shared -fPIC -std=c++11

OBJS=link.o datagram.o packet.o slave.o syncmanager.o fmmu.o

all: libethercat.so

link.o: link.cpp link.h
datagram.o: datagram.cpp datagram.h
packet.o: packet.cpp packet.h
slave.o: slave.cpp slave.h
syncmanager.o: syncmanager.cpp syncmanager.h
fmmu.o: fmmu.cpp fmmu.h

libethercat.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libethercat.so $^

.PHONY: clean

clean:
	rm -rf *~ $(OBJS) libethercat.so