CXX=g++
LD=g++
OBJS=send_test.o

CXXFLAGS=-I./ethercat
LDFLAGS=-L./ethercat -lethercat -lboost_system -lboost_thread

all: send_test

send_test: $(OBJS) ethercat/libethercat.so
	$(LD) $(LDFLAGS) -o $@ $^

ethercat/libethercat.so:
	make -C ethercat

.PHONY: clean test

test: send_test
	LD_LIBRARY_PATH=ethercat/ ./send_test

clean:
	make -C ethercat clean
	rm -rf *~ $(OBJS) send_test

