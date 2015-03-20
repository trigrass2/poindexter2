#include <link.h>
#include <slave.h>
#include <canopen.h>
#include <iostream>
#include <iomanip>

#include <datagram_address.h>
#include <packet.h>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define EEPROM_READ_SIZE 4096

// Borrowed from Beckhoff AX2xxx.xml
// I assume these can be set to anything, but these seem to be sensible defaults
#define AX2000_MBOX_OUT_ADDR 0x1800
#define AX2000_MBOX_OUT_SIZE 512
#define AX2000_MBOX_IN_ADDR  0x1c00
#define AX2000_MBOX_IN_SIZE  512

#define AX2000_PDO_OUT_ADDR 0x1100
#define AX2000_PDO_OUT_SIZE 6
#define AX2000_PDO_IN_ADDR  0x1140
#define AX2000_PDO_IN_SIZE  6

#define AX2000_FMMU_OUT_PHYSADDR 0x1100
#define AX2000_FMMU_OUT_LENGTH   8
#define AX2000_FMMU_OUT_LOGICALADDR 0x10000

#define AX2000_FMMU_IN_PHYSADDR 0x1140
#define AX2000_FMMU_IN_LENGTH 8
#define AX2000_FMMU_IN_LOGICALADDR 0x10000

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

	// Reset system time on the slave
	uint8_t data[32] = {0};
	slave->WriteData(0x910, data, 32);
	slave->WriteData(0x981, data, 1);
	slave->WriteShort(0x930, 0x1000);
	slave->WriteShort(0x934, 0x0c00);

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
	//uint32_t retVal = can->ReadSDO(0x1100, 0);
	std::cout << "State: " << retVal << std::endl;
	can->WriteSDO(0x6060, 0, 0xFE, 1);
	retVal = can->ReadSDO(0x6061, 0);
	std::cout << "State: " << retVal << std::endl;

	retVal = can->ReadSDO(0x60c2, 1);
	std::cout << "Interpolation time units: " << retVal << std::endl;
	// This should be 20ms, I believe...
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
	outPDO->ECATLatch(true);
	outPDO->PDILatch(true); // I'm hoping this does something...
	outPDO->Enable();

	inPDO->Disable();
	inPDO->StartAddr(AX2000_PDO_IN_ADDR);
	inPDO->Length(AX2000_PDO_IN_SIZE);
	inPDO->OperationMode(EtherCAT::SyncManager::OpMode::Buffered);
	inPDO->TransferDirection(EtherCAT::SyncManager::Direction::Read);
	inPDO->PDIInterrupt(true);
	inPDO->Enable();

	// Now the FMMUs
	EtherCAT::FMMU::Pointer outFMMU = slave->FMMUOut();
	EtherCAT::FMMU::Pointer inFMMU  = slave->FMMUIn();

	outFMMU->Disable();
	outFMMU->LogicalStart(AX2000_FMMU_OUT_LOGICALADDR);
	outFMMU->Length(AX2000_FMMU_OUT_LENGTH);
	outFMMU->LogicalStartBit(0);
	outFMMU->LogicalStopBit(7);
	outFMMU->PhysicalStart(AX2000_FMMU_OUT_PHYSADDR);
	outFMMU->PhysicalStartBit(0);
	outFMMU->WriteActive(true);
	outFMMU->ReadActive(false);
	outFMMU->Enable();

	inFMMU->Disable();
	inFMMU->LogicalStart(AX2000_FMMU_IN_LOGICALADDR);
	inFMMU->Length(AX2000_FMMU_IN_LENGTH);
	inFMMU->LogicalStartBit(0);
	inFMMU->LogicalStopBit(7);
	inFMMU->PhysicalStart(AX2000_FMMU_IN_PHYSADDR);
	inFMMU->PhysicalStartBit(0);
	inFMMU->WriteActive(false);
	inFMMU->ReadActive(true);
	inFMMU->Enable();


	// Probe the PDO stuff
	//std::cout << "NumSM: " << can->ReadSDO(0x1c00, 0) << std::endl;
	//std::cout << "SM0" << can->ReadSDO(0x1c00, 1) << std::endl;
	//std::cout << "SM1" << can->ReadSDO(0x1c00, 2) << std::endl;
	//std::cout << "SM2" << can->ReadSDO(0x1c00, 3) << std::endl;

	// Set up the PDO mappings...
	retVal = can->ReadSDO(0x1c12, 0);
	std::cout << "Test: " << std::hex << retVal << std::dec << std::endl;
	retVal = can->ReadSDO(0x1c12, 1);
	std::cout << "Test: " << std::hex << retVal << std::dec << std::endl;
	can->WriteSDO(0x1c12, 1, 0x1702, 2);
	can->WriteSDO(0x1c13, 1, 0x1b01, 2);
	can->WriteSDO(0x1c12, 0, 1, 2);
	can->WriteSDO(0x1c13, 0, 1, 2);

	retVal = can->ReadSDO(0x1001, 0);
	std::cout << "Error: " << std::hex << retVal << std::dec << std::endl;

	slave->ChangeState(EtherCAT::Slave::State::SAFEOP);

	// Give the slave some time to write data
	for(volatile int i = 0; i < 1000000; i++);

	// Now start firing data and see what happens!
	int numIter = 0;

	// Set up the timer
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::milliseconds(20));
	
	while(numIter++ < 1000)
	{
		uint8_t* data;
		EtherCAT::Datagram::Pointer dgram(new EtherCAT::DatagramLRW(8, AX2000_FMMU_OUT_LOGICALADDR));
		data = dgram->payload_ptr();

		uint32_t v = 0;
		data[0] = v & 0xFF;
		data[1] = (v >> 8) & 0xFF;
		data[2] = (v >> 16) & 0xFF;
		data[3] = (v >> 24) & 0xFF;

		// Disable the voltage...
		uint16_t control = 0x6;
		data[4] = control & 0xFF;
		data[5] = (control >> 8) & 0xFF;

		EtherCAT::Packet pkt;
		pkt.AddDatagram(dgram);
		pkt.SendReceive(link);

		uint32_t position = 0;
		position |= data[0];
		position |= (data[1] << 8);
		position |= (data[2] << 16);
		position |= (data[3] << 24);

		uint16_t status = 0;
		status |= data[4];
		status |= (data[5] << 8);

		// Wait for 20ms
		t.wait();
		t.expires_at(t.expires_at() + boost::posix_time::milliseconds(20));

		std::cout << "Position: " << position << std::endl;
		std::cout << "Status: " << std::hex << status << std::dec << std::endl;
	}

	//return 0;
		
	slave->ChangeState(EtherCAT::Slave::State::OP);

	// Transition to OP?
	//slave->ChangeStateASync(EtherCAT::Slave::State::OP);

	uint16_t statusWord = 0;
	int switchOnCycles = 0;
	int switchedOnCycles = 0;
	int enabledCycles = 0;
	int state = 0; // 0 = off, 1 = on, 2 = operational, 3 = enabled
	uint16_t controlWord = 0x6;
	uint16_t v = 0;
	while(1)
	{
		// Wait for 20ms
		t.wait();
		t.expires_at(t.expires_at() + boost::posix_time::milliseconds(20));

		uint8_t* data;
		EtherCAT::Datagram::Pointer dgram(new EtherCAT::DatagramLRW(6, AX2000_FMMU_OUT_LOGICALADDR));
		data = dgram->payload_ptr();

		data[0] = v & 0xFF;
		data[1] = (v >> 8) & 0xFF;
		data[2] = (v >> 16) & 0xFF;
		data[3] = (v >> 24) & 0xFF;

		// Disable the voltage...
		data[4] = controlWord & 0xFF;
		data[5] = (controlWord >> 8) & 0xFF;

		EtherCAT::Packet pkt;
		pkt.AddDatagram(dgram);
		pkt.SendReceive(link);

		uint32_t position = 0;
		position |= data[0];
		position |= (data[1] << 8);
		position |= (data[2] << 16);
		position |= (data[3] << 24);

		statusWord |= data[4];
		statusWord |= (data[5] << 8);

		// Is it ready to switch on?
		if(statusWord & 0x1 && switchOnCycles < 100)
			switchOnCycles++;
		else if(switchOnCycles < 100)
			switchOnCycles = 0;

		if(switchOnCycles == 100 && state == 0)
		{
			// Enable the output.
			// I'm kinda scared of this bit...
			controlWord |= 0x1;
			state = 1;
		}

		if(statusWord & 0x2 && switchedOnCycles < 100)
			switchedOnCycles++;
		else if(switchedOnCycles < 100)
			switchedOnCycles = 0;

		if(switchedOnCycles == 100 && state == 1)
		{
			// Operation enable...
			controlWord |= 0x8;
			state = 2;
		}

		if(statusWord & 0x4 && enabledCycles < 10)
			enabledCycles++;
		else if(enabledCycles < 10)
			enabledCycles = 0;

		if(enabledCycles == 10 && state == 2)
		{
			v = 1000;
			state = 3;
		}


		std::cout << "Position: " << position << std::endl;
		std::cout << "Status: " << std::hex << statusWord << std::dec << std::endl;
	}

	return 0;
}
