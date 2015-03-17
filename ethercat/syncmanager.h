#ifndef __SYNCMANAGER_H__
#define __SYNCMANAGER_H__

//#include "slave.h"
#include <memory>
#include <boost/cstdint.hpp>

namespace EtherCAT
{

// Ugh. I screwed up. There has to be a better way to do this...
class Slave;
typedef std::shared_ptr<Slave> SlavePointer;

class SyncManager
{
public:
	enum class OpMode
	{
		Buffered,
		Mailbox
	};

	enum class Direction
	{
		Read,   // ECAT read, PDI write
		Write   // ECAT write, PDI read
	};

	typedef std::shared_ptr<SyncManager> Pointer;
	SyncManager(SlavePointer slave, uint8_t syncManagerIndex);

	uint16_t  StartAddr()         { return startAddr; }
	uint16_t  Length()            { return length; }
	OpMode    OperationMode()     { return control_opmode; }
	Direction TransferDirection() { return control_direction; }
	bool      Enabled()           { return enabled; }

	void StartAddr(uint16_t addr);
	void Length(uint16_t length);
	void OperationMode(OpMode newMode);
	void TransferDirection(Direction newDir);

	void Enable();
	void Disable();


private:
	SlavePointer slave;
	int syncManagerIndex;

	// SM registers
	uint16_t startAddr;
	uint16_t length;

	OpMode control_opmode;
	Direction control_direction;
	bool control_ecat_intr;
	bool control_pdi_intr;
	bool watchdog_trigger;
	
	bool enabled;
	bool ecat_latch_event;
	bool pdi_latch_event;
};

}

#endif
