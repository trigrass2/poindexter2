#include <link.h>
#include <slave.h>
#include <iostream>

#define EEPROM_READ_SIZE 4096

// Borrowed from Beckhoff AX2xxx.xml
// I assume these can be set to anything, but these seem to be sensible defaults
#define AX2000_MBOX_OUT_ADDR 0x1800
#define AX2000_MBOX_OUT_SIZE 512
#define AX2000_MBOX_IN_ADDR  0x1c00
#define AX2000_MBOX_IN_SIZE  512

int main()
{
	std::cout << "Hello World!\n";
	unsigned char macAddr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

	EtherCAT::Link::Pointer link(new EtherCAT::Link("enp4s0", macAddr));
	
	int nSlaves = EtherCAT::Slave::NumSlaves(link);
	std::cout << "Found " << nSlaves << " slaves" << std::endl;

	// We're only going to play with the first slave for now.
	EtherCAT::Slave::Pointer slave(new EtherCAT::Slave(link, 0));
	slave->Init();

	// Get the syncmanagers and set them up
	EtherCAT::SyncManager::Pointer outMBox = slave->SyncManagerOutMBox();
	EtherCAT::SyncManager::Pointer inMBox = slave->SyncManagerInMBox();

	outMBox->Disable();
	outMBox->StartAddr(AX2000_MBOX_OUT_ADDR);
	outMBox->Length(AX2000_MBOX_OUT_SIZE);
	outMBox->OperationMode(EtherCAT::SyncManager::OpMode::Mailbox);
	outMBox->TransferDirection(EtherCAT::SyncManager::Direction::Write);
	outMBox->PDIInterrupt(true);
	outMBox->Enable();

	inMBox->Disable();
	inMBox->StartAddr(AX2000_MBOX_IN_ADDR);
	inMBox->Length(AX2000_MBOX_IN_SIZE);
	inMBox->OperationMode(EtherCAT::SyncManager::OpMode::Mailbox);
	inMBox->TransferDirection(EtherCAT::SyncManager::Direction::Read);
	inMBox->PDIInterrupt(true);
	inMBox->Enable();

	slave->ChangeState(EtherCAT::Slave::State::PREOP);

	// Write dummy data to a mailbox to test
	uint8_t dummyCAN[512 - 6];
	uint16_t coeHeader;
	coeHeader = 0x1000;
	dummyCAN[0] = coeHeader & 0xFF;
	dummyCAN[1] = (coeHeader >> 8) & 0xFF;

	if(outMBox->MailboxFull())
	{
		std::cout << "OH NO" << std::endl;
		return 1;
	}

	outMBox->WriteMailbox(EtherCAT::SyncManager::MailboxType::CoE, dummyCAN, 512 - 6);;
	while(outMBox->MailboxFull()) std::cout << "Mailbox pending..." << std::endl;
	std::cout << "Mailbox written..." << std::endl;
	while(!inMBox->MailboxFull()) std::cout << "Reading mailbox..." << std::endl;
	std::cout << "Mailbox done" << std::endl;

	return 0;
}
