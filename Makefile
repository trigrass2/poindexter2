CXX=g++
LD=g++
OBJS=send_test.o

CXXFLAGS=-I./ethercat
LDFLAGS=-L./ethercat -lethercat -lboost_system -lboost_thread

all: send_test

.PHONY: libethercat clean

send_test: $(OBJS) libethercat
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

libethercat:
	make -C ethercat

clean:
	make -C ethercat clean
	rm -rf *~ $(OBJS) send_test

