#include "syncmanager.h"
#include "slave_registers.h"
#include "slave.h"

#include <stdexcept>
#include <iostream>

using namespace EtherCAT;

SyncManager::SyncManager(Slave::Pointer slave, uint8_t syncManagerIndex) : slave(slave), syncManagerIndex(syncManagerIndex)
{
	// Assert we can actually do this...
	if(syncManagerIndex >= slave->NumSyncManager())
		throw std::out_of_range("syncManagerIndex above number of sync managers");

	// Read the current configuration from the SyncManager
	this->startAddr = slave->ReadShort(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_STARTADDR, syncManagerIndex));
	this->length = slave->ReadShort(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_LENGTH, syncManagerIndex));

	std::cerr << "\tSyncmanager " << (int)syncManagerIndex;
	std::cerr << " addr 0x" << std::hex << this->startAddr;
	std::cerr << " len " << std::dec << this->length;

	// Read the control word
	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex));

	// And unpack
	switch(control & SLAVE_SYNCMANAGER_CONTROL_OPMODE_MASK)
	{
	case SLAVE_SYNCMANAGER_CONTROL_OPMODE_BUFFERED:
		control_opmode = OpMode::Buffered;
		std::cerr << " BUFFER";
		break;
	case SLAVE_SYNCMANAGER_CONTROL_OPMODE_MAILBOX:
		control_opmode = OpMode::Mailbox;
		std::cerr << " MAILBOX";
		break;
	default:
		throw SlaveException("Unknown SyncManager operation mode");
	}

	switch(control & SLAVE_SYNCMANAGER_CONTROL_DIRECTION_MASK)
	{
	case SLAVE_SYNCMANAGER_CONTROL_DIRECTION_READ:
		control_direction = Direction::Read;
		std::cerr << " READ";
		break;
	case SLAVE_SYNCMANAGER_CONTROL_DIRECTION_WRITE:
		control_direction = Direction::Write;
		std::cerr << " WRITE";
		break;
	default:
		throw SlaveException("Unknown SyncManager direction");
	}

	control_ecat_intr = control & SLAVE_SYNCMANAGER_CONTROL_INTR_ECAT_MASK;
	control_pdi_intr = control & SLAVE_SYNCMANAGER_CONTROL_INTR_PDI_MASK;
	watchdog_trigger = control & SLAVE_SYNCMANAGER_CONTROL_WATCHDOG_MASK;

	if(control_ecat_intr)
		std::cerr << " ECAT_INTR";
	if(control_pdi_intr)
		std::cerr << " PDI_INTR";
	if(watchdog_trigger)
		std::cerr << " WATCHDOG";

	// We don't need to read the status register yet

	// Read the activate register
	uint8_t activate = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex));

	enabled = activate & SLAVE_SYNCMANAGER_ACTIVATE_ACTIVE_MASK;
	ecat_latch_event = activate & SLAVE_SYNCMANAGER_ACTIVATE_ECAT_LATCH_MASK;
	pdi_latch_event = activate & SLAVE_SYNCMANAGER_ACTIVATE_PDI_LATCH_MASK;

	if(enabled)
		std::cerr << " ENABLED";
	if(ecat_latch_event)
		std::cerr << " ECAT_LATCH";
	if(pdi_latch_event)
		std::cerr << " PDI_LATCH";

	std::cerr << std::endl;
}
