CXX=g++
LD=g++
OBJS=flexpicker_master.o flexpicker_velocitycontroller.o flexpicker_positioncontroller.o flexpicker_kinematics.o

CXXFLAGS=-I../ethercat -std=c++11 -fPIC
LDFLAGS=-fPIC -shared -L../ethercat -lethercat -lboost_system -lboost_thread -std=c++11

all: libflexpicker.so

flexpicker_master.o: flexpicker_master.h flexpicker_master.cpp
flexpicker_velocitycontroller.o: flexpicker_velocitycontroller.h flexpicker_velocitycontroller.cpp
flexpicker_positioncontroller.o: flexpicker_positioncontroller.h flexpicker_positioncontroller.cpp
flexpicker_kinematics.o: flexpicker_kinematics.h flexpicker_kinematics.cpp

.PHONY: libethercat clean

libflexpicker.so: $(OBJS)
	$(LD) $(LDFLAGS) -o libflexpicker.so $^

libethercat:
	make -C ../ethercat

clean:
	rm -rf *~ $(OBJS) send_test

