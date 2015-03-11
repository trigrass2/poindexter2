CXX=g++
LD=g++
CXXFLAGS=-fPIC
LDFLAGS=-shared -fPIC

OBJS=link.o

all: libethercat.so

link.o: link.cpp link.h

libethercat.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libethercat.so $^

.PHONY: clean

clean:
	rm -rf *~ $(OBJS) libethercat.so