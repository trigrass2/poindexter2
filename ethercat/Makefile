CXX=g++
LD=g++
CXXFLAGS=-fPIC
LDFLAGS=-shared -fPIC

OBJS=link.o ethernet.o

all: libethercat.so

libethercat.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libethercat.so $^

.PHONY: clean

clean:
	rm -rf *~ $(OBJS) libethercat.so
