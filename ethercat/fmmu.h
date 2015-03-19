#ifndef __FMMU_H__
#define __FMMU_H__

//#include "slave.h"
#include <memory>

namespace EtherCAT
{

class Slave;
typedef std::shared_ptr<Slave> SlavePointer;

class FMMU
{
public:
	typedef std::shared_ptr<FMMU> Pointer;

	FMMU(SlavePointer slave, uint8_t fmmuIndex);

	uint32_t LogicalStart()     { return logicalStart; }
	uint16_t Length()           { return length; }
	uint8_t  LogicalStartBit()  { return logicalStartBit; }
	uint8_t  LogicalStopBit()   { return logicalStopBit; }
	uint16_t PhysicalStart()    { return physicalStart; }
	uint8_t  PhysicalStartBit() { return physicalStartBit; }
	bool ReadActive()           { return readActive; }
	bool WriteActive()          { return writeActive; }
	bool Enabled()              { return enabled; }

	void LogicalStart(uint32_t logicalStart);
	void Length(uint16_t length);
	void LogicalStartBit(uint8_t startBit);
	void LogicalStopBit(uint8_t stopBit);
	void PhysicalStart(uint16_t physicalStart);
	void PhysicalStartBit(uint8_t startBit);
	void ReadActive(bool read);
	void WriteActive(bool write);
	void Enable();
	void Disable();

private:
	SlavePointer slave;
	uint8_t fmmuIndex;

	uint32_t logicalStart;
	uint16_t length;
	uint8_t logicalStartBit;
	uint8_t logicalStopBit;
	uint16_t physicalStart;
	uint8_t physicalStartBit;

	bool readActive;
	bool writeActive;
	bool enabled;
};

}

#endif
