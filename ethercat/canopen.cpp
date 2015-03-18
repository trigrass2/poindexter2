#include "canopen.h"

using namespace EtherCAT;

CANopen::CANopen(SyncManager::Pointer outMBox, SyncManager::Pointer inMBox) : 
	outMBox(outMBox), inMBox(inMBox)
{
}

CANopen::~CANopen()
{

}

void CANopen::WriteSDO(uint16_t index, uint8_t subindex, uint32_t data)
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

	// Pack the data
	packet[6] = data & 0xFF;
	packet[7] = (data >> 8) & 0xFF;
	packet[8] = (data >> 16) & 0xFF;
	packet[9] = (data >> 24) & 0xFF;

	// Write it!
	outMBox->WriteMailbox(EtherCAT::SyncManager::MailboxType::CoE, packet, CANOPEN_SDO_SIZE);
}

uint32_t CANopen::ReadSDO(uint16_t index, uint8_t subindex)
{
	// Block until the response is there.
	while(!inMBox->MailboxFull());

	// Receive and unpack
	uint8_t packet[CANOPEN_SDO_SIZE];
	EtherCAT::SyncManager::MailboxType mboxType = EtherCAT::SyncManager::MailboxType::Vendor;
	inMBox->ReadMailbox(&mboxType, packet, CANOPEN_SDO_SIZE);

	if(mboxType != EtherCAT::SyncManager::MailboxType::CoE)
		throw SlaveException("Data read from SyncManager is not a CoE packet");

	// TODO: Rest of this...

	return 0;
}
