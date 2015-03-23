CXX=g++
LD=g++
OBJS=send_test.o

CXXFLAGS=-I./ethercat -I./flexpicker -std=c++11
LDFLAGS=-L./ethercat -L./flexpicker -lethercat -lflexpicker -lboost_system -lboost_thread -std=c++11

all: send_test

.PHONY: libethercat flexpicker clean

send_test: $(OBJS) libethercat flexpicker
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

libethercat:
	make -C ethercat

flexpicker:
	make -C flexpicker


clean:
	make -C ethercat clean
	make -C flexpicker clean
	rm -rf *~ $(OBJS) send_test

