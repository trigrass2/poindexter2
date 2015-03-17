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

	outMBox->StartAddr(AX2000_MBOX_OUT_ADDR);
	outMBox->Length(AX2000_MBOX_OUT_SIZE);
	outMBox->OperationMode(EtherCAT::SyncManager::OpMode::Mailbox);
	outMBox->TransferDirection(EtherCAT::SyncManager::Direction::Write);

	inMBox->StartAddr(AX2000_MBOX_IN_ADDR);
	inMBox->Length(AX2000_MBOX_IN_SIZE);
	inMBox->OperationMode(EtherCAT::SyncManager::OpMode::Mailbox);
	inMBox->TransferDirection(EtherCAT::SyncManager::Direction::Read);

	slave->ChangeState(EtherCAT::Slave::State::PREOP);


	return 0;
}
