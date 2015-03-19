#include "fmmu.h"
#include "slave.h"
#include "slave_registers.h"

#include <iostream>

using namespace EtherCAT;

FMMU::FMMU(Slave::Pointer slave, uint8_t fmmuIndex) :
	slave(slave), fmmuIndex(fmmuIndex)
{
	// Read out the current configuration
	logicalStart = slave->ReadWord(FMMU_ADDR(SLAVE_FMMU_LOGICAL_START, fmmuIndex));
	length = slave->ReadShort(FMMU_ADDR(SLAVE_FMMU_LENGTH, fmmuIndex));
	logicalStartBit = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_LOGICAL_START_BIT, fmmuIndex));
	logicalStopBit = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_LOGICAL_STOP_BIT, fmmuIndex));
	physicalStart = slave->ReadShort(FMMU_ADDR(SLAVE_FMMU_PHYSICAL_START, fmmuIndex));
	physicalStartBit = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_PHYSICAL_START_BIT, fmmuIndex));

	uint8_t typeReg = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_TYPE, fmmuIndex));
	readActive = typeReg & SLAVE_FMMU_TYPE_READ_MASK;
	writeActive = typeReg & SLAVE_FMMU_TYPE_WRITE_MASK;

	uint8_t enableReg = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_ACTIVATE, fmmuIndex));
	enabled = enableReg & SLAVE_FMMU_ACTIVATE_ACTIVE_MASK;

	std::cerr << "FMMU " << fmmuIndex << " start 0x" << std::hex << logicalStart << " length: 0x" << length
	          << " startbit: " << (int)logicalStartBit << " stopbit: " << (int)logicalStopBit
	          << " physicalStart: 0x" << physicalStart << " startbit: " << physicalStartBit;
	if(readActive)
		std::cerr << " READ";
	if(writeActive)
		std::cerr << " WRITE";
	if(enabled)
		std::cerr << " ENABLED";
	std::cerr << std::endl;
}

void FMMU::LogicalStart(uint32_t logicalStart)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	slave->WriteWord(FMMU_ADDR(SLAVE_FMMU_LOGICAL_START, fmmuIndex), logicalStart);
	this->logicalStart = logicalStart;
}

void FMMU::Length(uint16_t length)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	slave->WriteShort(FMMU_ADDR(SLAVE_FMMU_LENGTH, fmmuIndex), length);
	this->length = length;
}

void FMMU::LogicalStartBit(uint8_t logicalStartBit)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_LOGICAL_START_BIT, fmmuIndex), logicalStartBit);
	this->logicalStartBit = logicalStartBit;
}

void FMMU::LogicalStopBit(uint8_t logicalStopBit)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_LOGICAL_STOP_BIT, fmmuIndex), logicalStopBit);
	this->logicalStopBit = logicalStopBit;
}

void FMMU::PhysicalStart(uint16_t physicalStart)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	slave->WriteShort(FMMU_ADDR(SLAVE_FMMU_PHYSICAL_START, fmmuIndex), physicalStart);
	this->physicalStart = physicalStart;
}

void FMMU::PhysicalStartBit(uint8_t physicalStartBit)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_PHYSICAL_START_BIT, fmmuIndex), physicalStartBit);
	this->physicalStartBit = physicalStartBit;
}

void FMMU::ReadActive(bool active)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	uint8_t typeReg = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_TYPE, fmmuIndex));
	typeReg &= ~SLAVE_FMMU_TYPE_READ_MASK;
	if(active)
		typeReg |= SLAVE_FMMU_TYPE_READ_MASK;
	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_TYPE, fmmuIndex), typeReg);
	readActive = active;
}

void FMMU::WriteActive(bool active)
{
	if(enabled)
		throw SlaveException("Attempted to change enabled FMMU");
	uint8_t typeReg = slave->ReadByte(FMMU_ADDR(SLAVE_FMMU_TYPE, fmmuIndex));
	typeReg &= ~SLAVE_FMMU_TYPE_WRITE_MASK;
	if(active)
		typeReg |= SLAVE_FMMU_TYPE_WRITE_MASK;
	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_TYPE, fmmuIndex), typeReg);
	writeActive = active;
}

void FMMU::Enable()
{
	if(enabled)
		return;

	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_ACTIVATE, fmmuIndex), SLAVE_FMMU_ACTIVATE_ACTIVE_MASK);
	enabled = true;
}

void FMMU::Disable()
{
	if(!enabled)
		return;

	slave->WriteByte(FMMU_ADDR(SLAVE_FMMU_ACTIVATE, fmmuIndex), 0);
	enabled = false;
}