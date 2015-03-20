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

void SyncManager::StartAddr(uint16_t addr)
{
	if(enabled)
		throw SlaveException("Tried to change StartAddr when SyncManager enabled");
	
	this->startAddr = addr;
	slave->WriteShort(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_STARTADDR, syncManagerIndex), startAddr);
}

void SyncManager::Length(uint16_t length)
{
	if(enabled)
		throw SlaveException("Tried to change Length when SyncManager enabled");

	this->length = length;
	slave->WriteShort(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_LENGTH, syncManagerIndex), length);
}
	
void SyncManager::OperationMode(OpMode newMode)
{
	if(enabled)
		throw SlaveException("Tried to change OperationMode when SyncManager enabled");

	// Read control
	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex));
	control &= ~SLAVE_SYNCMANAGER_CONTROL_OPMODE_MASK;

	switch(newMode)
	{
	case OpMode::Buffered:
		control |= SLAVE_SYNCMANAGER_CONTROL_OPMODE_BUFFERED;
		break;
	case OpMode::Mailbox:
		control |= SLAVE_SYNCMANAGER_CONTROL_OPMODE_MAILBOX;
		break;
	}

	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex), control);
	this->control_opmode = newMode;
}

void SyncManager::TransferDirection(Direction dir)
{
	if(enabled)
		throw SlaveException("Tried to change TransferDirection when SyncManager enabled");

	// Read control
	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex));
	control &= ~SLAVE_SYNCMANAGER_CONTROL_DIRECTION_MASK;

	switch(dir)
	{
	case Direction::Read:
		control |= SLAVE_SYNCMANAGER_CONTROL_DIRECTION_READ;
		break;
	case Direction::Write:
		control |= SLAVE_SYNCMANAGER_CONTROL_DIRECTION_WRITE;
		break;
	}

	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex), control);
	this->control_direction = dir;
}

void SyncManager::Enable()
{
	if(enabled)
		return;

	uint8_t activate = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex));
	activate |= SLAVE_SYNCMANAGER_ACTIVATE_ACTIVE_MASK;
	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex), activate);

	enabled = false;
}

void SyncManager::Disable()
{
	if(!enabled)
		return;

	uint8_t activate = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex));
	activate &= ~SLAVE_SYNCMANAGER_ACTIVATE_ACTIVE_MASK;
	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex), activate);

	enabled = false;
}

void SyncManager::ECATInterrupt(bool intr)
{
	if(enabled)
		throw SlaveException("Tried to change ECATInterrupt when SyncManager enabled");

	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex));
	control &= ~SLAVE_SYNCMANAGER_CONTROL_INTR_ECAT_MASK;

	if(intr)
		control |= SLAVE_SYNCMANAGER_CONTROL_INTR_ECAT_MASK;

	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex), control);
	this->control_ecat_intr = intr;
}

void SyncManager::PDIInterrupt(bool intr)
{
	if(enabled)
		throw SlaveException("Tried to change PDIInterrupt when SyncManager enabled");

	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex));
	control &= ~SLAVE_SYNCMANAGER_CONTROL_INTR_PDI_MASK;

	if(intr)
		control |= SLAVE_SYNCMANAGER_CONTROL_INTR_PDI_MASK;

	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_CONTROL, syncManagerIndex), control);
	this->control_pdi_intr = intr;
}

void SyncManager::ECATLatch(bool latch)
{
	if(enabled)
		throw SlaveException("Tried to change ECATLatch when SyncManager enabled");

	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex));
	control &= ~SLAVE_SYNCMANAGER_ACTIVATE_ECAT_LATCH_MASK;

	if(latch)
		control |= SLAVE_SYNCMANAGER_ACTIVATE_ECAT_LATCH_MASK;

	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex), control);
	this->ecat_latch_event = latch;
}

void SyncManager::PDILatch(bool latch)
{
	if(enabled)
		throw SlaveException("Tried to change PDI:atch when SyncManager enabled");

	uint8_t control = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex));
	control &= ~SLAVE_SYNCMANAGER_ACTIVATE_PDI_LATCH_MASK;

	if(latch)
		control |= SLAVE_SYNCMANAGER_ACTIVATE_PDI_LATCH_MASK;

	slave->WriteByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_ACTIVATE, syncManagerIndex), control);
	this->pdi_latch_event = latch;
}

bool SyncManager::MailboxFull()
{
	if(control_opmode != OpMode::Mailbox)
		throw SlaveException("Cannot query mailboxfull; SyncManager is not a mailbox");

	uint8_t status = slave->ReadByte(SYNCMANAGER_ADDR(SLAVE_SYNCMANAGER_STATUS, syncManagerIndex));
	if(status & SLAVE_SYNCMANAGER_STATUS_MAILBOX_FULL_MASK)
		return true;
	else
		return false;
}

void SyncManager::WriteMailbox(MailboxType type, uint8_t* payload, int length)
{
	if(control_opmode != OpMode::Mailbox)
		throw SlaveException("Cannot write to a mailbox on a non-mailbox SyncManager");
	if(control_direction != Direction::Write)
		throw SlaveException("Tried to write to a non-output SyncManager");

	// For the PDI to care, we seem to have to write the last byte for the SyncManager
	// Force this here...
	int payload_length = length;
	if(length + SYNCMANAGER_MAILBOX_HEADER_LEN < this->length)
		length = this->length - SYNCMANAGER_MAILBOX_HEADER_LEN;

	// Create the packet
	// TODO: Try and reduce the number of copies...
	// TODO: MAGIC NUMBERS. MAGIC NUMBERS EVERYWHERE.
	// TODO: This is (very) badly written. Sorry.
	uint8_t telegram[length + SYNCMANAGER_MAILBOX_HEADER_LEN];

	// Set the header
	uint16_t len_16bit = length;
	uint16_t destAddr = 0; //slave->SlaveAddress(); // TODO: Is this correct?

	telegram[0] = payload_length & 0xFF;
	telegram[1] = (payload_length >> 8) & 0xFF;
	telegram[2] = destAddr & 0xFF;
	telegram[3] = (destAddr >> 8) & 0xFF;
	telegram[4] = 0x0; // Channel reserved. Leave priority at 0

	switch(type)
	{
	case MailboxType::Vendor:
		telegram[5] = 0x1;
		break;
	case MailboxType::EoE:
		telegram[5] = 0x2;
		break;
	case MailboxType::CoE:
		telegram[5] = 0x3;
		break;
	case MailboxType::FoE:
		telegram[5] = 0x4;
		break;
	case MailboxType::SoE:
		telegram[5] = 0x5;
		break;
	}

	//telegram[5] |= (sequenceNumber << 1) & 0xE;

	// Copy the payload in
	uint8_t* telegramWritePointer = telegram;
	telegramWritePointer += SYNCMANAGER_MAILBOX_HEADER_LEN;

	while(length--)
	{
		if(payload_length)
		{
			*telegramWritePointer++ = *payload++;
			payload_length--;
		}
		else
			*telegramWritePointer++ = 0;
	}

	// Send it!
	slave->WriteData(this->startAddr, telegram, len_16bit + SYNCMANAGER_MAILBOX_HEADER_LEN);
}

int SyncManager::ReadMailbox(MailboxType* type, uint8_t* payload, int max_length)
{
	// TODO: Should these be std::logic_exceptions?
	// TODO: MORE MAGIC NUMBERS. YET MORE MAGIC NUMBERS EVERYWHERE.
	if(control_opmode != OpMode::Mailbox)
		throw SlaveException("Cannot read from a mailbox on a non-mailbox SyncManager");
	if(control_direction != Direction::Read)
		throw SlaveException("Tried to read to a non-input SyncManager");

	// Make sure there's something there...
	if(!this->MailboxFull())
		return 0; // TODO: Should this be an exception?

	// Read the whole mailbox.
	// This is because we need to read from the last byte to trigger the "done"
	uint8_t mailboxMessage[this->length];

	slave->ReadData(this->startAddr, mailboxMessage, this->length);

	// Now parse and copy out
	// Endian swaps...
	uint16_t payloadLength = mailboxMessage[0];
	payloadLength |= mailboxMessage[1] << 8;

	uint16_t stationAddr = mailboxMessage[2];
	stationAddr |= mailboxMessage[3] << 8;

	// Can ignore mailboxMessage[4]

	uint8_t messageType = mailboxMessage[5] & 0xF;
	MailboxType returnType;

	switch(messageType)
	{
	case 0x1:
	case 0xF:
		returnType = MailboxType::Vendor;
		break;
	case 0x2:
		returnType = MailboxType::EoE;
		break;
	case 0x3:
		returnType = MailboxType::CoE;
		break;
	case 0x4:
		returnType = MailboxType::FoE;
		break;
	case 0x5:
		returnType = MailboxType::SoE;
		break;
	default:
		throw SlaveException("Unknown SyncManager mailbox type");
	}

	if(type != NULL)
		*type = returnType;

	// Copy out the contents
	uint8_t* messageReadPointer = mailboxMessage;
	messageReadPointer += SYNCMANAGER_MAILBOX_HEADER_LEN;
	int numBytes = 0;
	while(numBytes < max_length && numBytes < payloadLength)
	{
		*payload++ = *messageReadPointer++;
		numBytes++;
	}

	return numBytes;
}

void SyncManager::WriteBuffered(uint8_t* payload, int payload_length)
{
	if(control_opmode != OpMode::Buffered)
		throw SlaveException("Cannot write to a buffer on a non-buffer SyncManager");
	if(control_direction != Direction::Write)
		throw SlaveException("Tried to write to a non-output SyncManager");
	if(payload_length != length)
		throw SlaveException("WriteBuffered is not writing the correct amount of information!");

	// Don't even need to tag this, just fire it in!
	slave->WriteData(this->startAddr, payload, payload_length);
}

void SyncManager::ReadBuffered(uint8_t* payload, int payload_length)
{
	if(control_opmode != OpMode::Buffered)
		throw SlaveException("Cannot write to a buffer on a non-buffer SyncManager");
	if(control_direction != Direction::Read)
		throw SlaveException("Tried to read from a non-input SyncManager");
	if(payload_length != length)
		throw SlaveException("WriteBuffered is not writing the correct amount of information!");

	// Don't even need to tag this, just fire it in!
	slave->ReadData(this->startAddr, payload, payload_length);
}