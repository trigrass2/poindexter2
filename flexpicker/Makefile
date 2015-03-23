CXX=g++
LD=g++
OBJS=flexpicker_master.o flexpicker_velocitycontroller.o

CXXFLAGS=-I../ethercat -std=c++11 -fPIC
LDFLAGS=-fPIC -shared -L../ethercat -lethercat -lboost_system -lboost_thread -std=c++11

all: libflexpicker.so

.PHONY: libethercat clean

libflexpicker.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libflexpicker.so $^

libethercat:
	make -C ../ethercat

clean:
	rm -rf *~ $(OBJS) send_test

