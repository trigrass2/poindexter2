#include <link.h>
#include <slave.h>
#include <canopen.h>
#include <iostream>

#define EEPROM_READ_SIZE 4096

// Borrowed from Beckhoff AX2xxx.xml
// I assume these can be set to anything, but these seem to be sensible defaults
#define AX2000_MBOX_OUT_ADDR 0x1800
#define AX2000_MBOX_OUT_SIZE 512
#define AX2000_MBOX_IN_ADDR  0x1c00
#define AX2000_MBOX_IN_SIZE  512

#define AX2000_PDO_OUT_ADDR 0x1100
#define AX2000_PDO_OUT_SIZE 8
#define AX2000_PDO_IN_ADDR  0x1140
#define AX2000_PDO_IN_SIZE  8

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


	// Create the CANopen wrapper and test
	EtherCAT::CANopen::Pointer can(new EtherCAT::CANopen(outMBox, inMBox));

	// Play with the time period
	// Try and shut the error up!
	uint32_t retVal = can->ReadSDO(0x6061, 0);
	std::cout << "State: " << retVal << std::endl;
	can->WriteSDO(0x6060, 0, 0xFE, 1);
	retVal = can->ReadSDO(0x6061, 0);
	std::cout << "State: " << retVal << std::endl;

	retVal = can->ReadSDO(0x60c2, 1);
	std::cout << "Interpolation time units: " << retVal << std::endl;
	can->WriteSDO(0x60c2, 1, 20, 1);

	retVal = can->ReadSDO(0x60c2, 1);
	std::cout << "Interpolation time units: " << retVal << std::endl;

	retVal = can->ReadSDO(0x60c2, 2);
	std::cout << "Interpolation time index: " << retVal << std::endl;
	can->WriteSDO(0x60c2, 2, -3, 1);

	retVal = can->ReadSDO(0x60c2, 2);
	std::cout << "Interpolation time index: " << retVal << std::endl;

	// Set up the other sync managers
	EtherCAT::SyncManager::Pointer outPDO = slave->SyncManagerOutPDO();
	EtherCAT::SyncManager::Pointer inPDO  = slave->SyncManagerInPDO();

	outPDO->Disable();
	outPDO->StartAddr(AX2000_PDO_OUT_ADDR);
	outPDO->Length(AX2000_PDO_OUT_SIZE);
	outPDO->OperationMode(EtherCAT::SyncManager::OpMode::Buffered);
	outPDO->TransferDirection(EtherCAT::SyncManager::Direction::Write);
	outPDO->PDIInterrupt(true);
	outPDO->Enable();

	inPDO->Disable();
	inPDO->StartAddr(AX2000_PDO_IN_ADDR);
	inPDO->Length(AX2000_PDO_IN_SIZE);
	inPDO->OperationMode(EtherCAT::SyncManager::OpMode::Buffered);
	inPDO->TransferDirection(EtherCAT::SyncManager::Direction::Read);
	inPDO->PDIInterrupt(true);
	inPDO->Enable();

	slave->ChangeState(EtherCAT::Slave::State::SAFEOP);

	return 0;
}
