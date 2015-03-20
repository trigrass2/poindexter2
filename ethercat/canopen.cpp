#include "canopen.h"

using namespace EtherCAT;

CANopen::CANopen(SyncManager::Pointer outMBox, SyncManager::Pointer inMBox) : 
	outMBox(outMBox), inMBox(inMBox)
{
}

CANopen::~CANopen()
{

}

void CANopen::WriteSDO(uint16_t index, uint8_t subindex, uint32_t data, int data_size)
{
	// Assert that we can actually write to the mailbox
	// If it's full, it means the slave hasn't read the previous
	// request. This is bad.
	if(outMBox->MailboxFull())
		throw SlaveException("Tried to write to a full mailbox. This implies the previous transaction did not complete successfully");

	// Create the packet!
	uint8_t packet[CANOPEN_SDO_SIZE];

	uint16_t coe_header = COE_HEADER_TYPE_SDO_REQUEST;

	// Don't need the number for this one
	packet[0] = coe_header & 0xFF;
	packet[1] = (coe_header >> 8) & 0xFF;

	// CANopen SDO header
	packet[2] = SDO_COMMAND_DOWNLOAD;

	// Also want to set the expedited bits.
	packet[2] |= SDO_EXPEDITED_MASK;
	packet[2] |= SDO_SIZE_SET_MASK;
	switch(data_size)
	{
	case 1:
		packet[2] |= SDO_SIZE_1;
		break;
	case 2:
		packet[2] |= SDO_SIZE_2;
		break;
	case 3:
		packet[2] |= SDO_SIZE_3;
		break;
	case 4:
		packet[2] |= SDO_SIZE_4;
		break;
	default:
		throw CANopenException("Invalid number of bytes to write to CANopen SDO");
	}

	// CANopen index
	packet[3] = index & 0xFF;
	packet[4] = (index >> 8) & 0xFF;

	// CANopen subindex
	packet[5] = subindex;

	// Ensure the data is zeroed
	packet[6] = data & 0xFF;;
	packet[7] = (data >> 8) & 0xFF;
	packet[8] = (data >> 16) & 0xFF;
	packet[9] = (data >> 24) & 0xFF;

	// Write it!
	outMBox->WriteMailbox(EtherCAT::SyncManager::MailboxType::CoE, packet, CANOPEN_SDO_SIZE);

	// Wait for results
	while(!inMBox->MailboxFull());

	// Read it back out again!
	EtherCAT::SyncManager::MailboxType t = EtherCAT::SyncManager::MailboxType::Vendor;
	inMBox->ReadMailbox(&t, packet, CANOPEN_SDO_SIZE);

	// Ensure it's the correct type
	if(t != EtherCAT::SyncManager::MailboxType::CoE)
		throw SlaveException("Wrote CoE, didn't receive a CoE packet...");

	// Is it OK?
	// Response type /should/ be SDO_COMMAND_UPLOAD again...
	if((packet[2] & SDO_COMMAND_MASK) == SDO_COMMAND_ABORT)
		throw CANopenException("Slave aborted read transfer");

	// Apparently that's what it should return *shrug*)
	if((packet[2] & SDO_COMMAND_MASK) != SDO_COMMAND_UPLOAD_SEGMENT)
		throw CANopenException("Slave did not respond to read with upload protocol");
}

uint32_t CANopen::ReadSDO(uint16_t index, uint8_t subindex)
{
	// Assert that we can actually write to the mailbox
	// If it's full, it means the slave hasn't read the previous
	// request. This is bad.
	if(outMBox->MailboxFull())
		throw SlaveException("Tried to write to a full mailbox. This implies the previous transaction did not complete successfully");

	// Create the packet!
	uint8_t packet[CANOPEN_SDO_SIZE];

	uint16_t coe_header = COE_HEADER_TYPE_SDO_REQUEST;

	// Don't need the number for this one
	packet[0] = coe_header & 0xFF;
	packet[1] = (coe_header >> 8) & 0xFF;

	// CANopen SDO header
	packet[2] = SDO_COMMAND_UPLOAD;

	// CANopen index
	packet[3] = index & 0xFF;
	packet[4] = (index >> 8) & 0xFF;

	// CANopen subindex
	packet[5] = subindex;

	// Ensure the data is zeroed
	packet[6] = 0;
	packet[7] = 0;
	packet[8] = 0;
	packet[9] = 0;

	// Write it!
	outMBox->WriteMailbox(EtherCAT::SyncManager::MailboxType::CoE, packet, CANOPEN_SDO_SIZE);

	// Wait for results
	while(!inMBox->MailboxFull());

	// Read it back out again!
	EtherCAT::SyncManager::MailboxType t = EtherCAT::SyncManager::MailboxType::Vendor;
	inMBox->ReadMailbox(&t, packet, CANOPEN_SDO_SIZE);

	// Ensure it's the correct type
	if(t != EtherCAT::SyncManager::MailboxType::CoE)
		throw SlaveException("Wrote CoE, didn't receive a CoE packet...");

	// Is it OK?
	// Response type /should/ be SDO_COMMAND_UPLOAD again...
	if((packet[2] & SDO_COMMAND_MASK) == SDO_COMMAND_ABORT)
		throw CANopenException("Slave aborted read transfer");
	if((packet[2] & SDO_COMMAND_MASK) != SDO_COMMAND_UPLOAD)
		throw CANopenException("Slave did not respond to read with upload protocol");

	// Get the data out and return
	uint32_t response = 0;
	response |= packet[6];
	response |= (packet[7] << 8);
	response |= (packet[8] << 16);
	response |= (packet[9] << 24);

	return response;
}

