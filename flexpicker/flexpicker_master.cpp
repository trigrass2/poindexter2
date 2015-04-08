#include "flexpicker_master.h"
#include "ax2000_params.h"
#include "flexpicker_kinematics.h"

#include <canopen_sdo.h>
#include <packet.h>
#include <datagram_broadcast.h>

#include <boost/cstdint.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace Flexpicker;

Master::Master(EtherCAT::Link::Pointer link) : link(link)
{
	// Create the communication manager
	manager = EtherCAT::CyclicCommunicationManager::Pointer(new EtherCAT::CyclicCommunicationManager(link, INTERPOLATION_PERIOD_MS));

	// Probe for the number of slaves...
	int numSlaves = EtherCAT::Slave::NumSlaves(manager);
	if(numSlaves < FLEXPICKER_SLAVES)
	  throw FlexPickerException("There are less slaves than expected...");

	// Init the slaves
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		EtherCAT::Slave::Pointer newSlave(new EtherCAT::Slave(manager, i));
		slaves[i] = newSlave;
	}

	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
		slaves[i]->Init();
}

Master::~Master()
{

}

void Master::Setup()
{
	// First, clear errors and go to the INIT state
	// Ideally, we'd like to broadcast these. Oh well.
	// Add static ChangeStateBroadcast/ClearErrorBroadcast to EtherCAT later?
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		//slaves[i]->ClearErrors();
		slaves[i]->ChangeState(EtherCAT::Slave::State::INIT, true);
	}

	// Start setting them up
	// In INIT, we need to set up the initial EtherCAT parameters
	// and mailbox SyncManagers
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		// Clear distributed clock settings.
		// I found this on the old control code. Clean it up later.
		uint8_t data[32] = {0};
		slaves[i]->WriteData(0x910, data, 32);
		slaves[i]->WriteData(0x981, data, 1);
		slaves[i]->WriteShort(0x930, 0x1000);
		slaves[i]->WriteShort(0x934, 0x0c00);
	}

	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		// Set up the SyncManagers
		EtherCAT::SyncManager::Pointer outMBox = slaves[i]->SyncManagerOutMBox();
		EtherCAT::SyncManager::Pointer inMBox  = slaves[i]->SyncManagerInMBox();

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

		slaves[i]->ChangeState(EtherCAT::Slave::State::PREOP);
	}

	// Everything should now be in PREOP state
	// Now set up the CANopen wrappers and get the PDO storage working
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		EtherCAT::CANopen::Pointer can(new EtherCAT::CANopen(slaves[i]->SyncManagerOutMBox(), slaves[i]->SyncManagerInMBox()));
		slaveCan[i] = can;

		// Defined in Beckhoff AX2xxx.xml
		can->SetOutputPDO(0x1702); // Velocity demand, control word
		can->SetInputPDO(0x1b03);  // Position actual, torque actual, status word

		// Set up the operation mode
		can->WriteSDO(SDO_I_OPERATION_MODE, 0, SDO_OPERATION_MODE_DIGITAL_SPEED, 1);
		//can->WriteSDO(SDO_I_OPERATION_MODE, 0, 1, 1);

		std::cout << "Velocity: " << can->ReadSDO(0x6081, 0) << std::endl;
		std::cout << "Motion type: " << std::hex << can->ReadSDO(0x2022, 3) << std::dec << std::endl;

		// Read out the MSSR
		std::cout << "MSSR: 0x" << std::hex << can->ReadSDO(0x1002, 0) << std::dec << std::endl;

		// Sanity...
		std::cout << "OpMode: 0x" << std::hex << can->ReadSDO(0x6061, 0) << std::dec << std::endl;

		// Ramp...
		std::cout << "Motion ramp: " << can->ReadSDO(0x6086, 0) << std::endl;

		std::cout << "Range limits: " << (int)can->ReadSDO(0x607b, 1) << " - " << (int)can->ReadSDO(0x607b, 2) << std::endl;
		std::cout << "Homing mode: " << can->ReadSDO(0x2024, 1) << std::endl;
		std::cout << "Target position: " << can->ReadSDO(0x607A, 0) << std::endl;

		// And the interpolation units
		can->WriteSDO(SDO_I_INTERPOLATION_PERIOD, SDO_SI_INTERPOLATION_PERIOD_UNIT, INTERPOLATION_PERIOD_MS, 1);
		can->WriteSDO(SDO_I_INTERPOLATION_PERIOD, SDO_SI_INTERPOLATION_PERIOD_INDEX, -3, 1);
		
		// Set up the other sync managers
		EtherCAT::SyncManager::Pointer outPDO = slaves[i]->SyncManagerOutPDO();
		EtherCAT::SyncManager::Pointer inPDO  = slaves[i]->SyncManagerInPDO();

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

		// And the FMMUs
		EtherCAT::FMMU::Pointer outFMMU = slaves[i]->FMMUOut();
		EtherCAT::FMMU::Pointer inFMMU  = slaves[i]->FMMUIn();

		uint32_t logicalAddr = AX2000_FMMU_LOGICALADDR + (i * AX2000_FMMU_LOGICALADDR_STRIDE);

		outFMMU->Disable();
		outFMMU->LogicalStart(logicalAddr);
		outFMMU->Length(AX2000_FMMU_OUT_LENGTH);
		outFMMU->LogicalStartBit(0);
		outFMMU->LogicalStopBit(7);
		outFMMU->PhysicalStart(AX2000_FMMU_OUT_PHYSADDR);
		outFMMU->PhysicalStartBit(0);
		outFMMU->WriteActive(true);
		outFMMU->ReadActive(false);
		outFMMU->Enable();

		inFMMU->Disable();
		inFMMU->LogicalStart(logicalAddr);
		inFMMU->Length(AX2000_FMMU_IN_LENGTH);
		inFMMU->LogicalStartBit(0);
		inFMMU->LogicalStopBit(7);
		inFMMU->PhysicalStart(AX2000_FMMU_IN_PHYSADDR);
		inFMMU->PhysicalStartBit(0);
		inFMMU->WriteActive(false);
		inFMMU->ReadActive(true);
		inFMMU->Enable();

		slaves[i]->ChangeState(EtherCAT::Slave::State::SAFEOP);
	}

	// All slaves should now be SAFEOP.
	// Start up the Velocity controllers and try and move to op!
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		uint32_t logicalAddr = AX2000_FMMU_LOGICALADDR + (i * AX2000_FMMU_LOGICALADDR_STRIDE);

		VelocityController::Pointer vc(new VelocityController(logicalAddr));
		vel[i] = vc;

		manager->RegisterCyclicController(vc);
	}

	manager->Run();
	
	boost::asio::io_service io;
	boost::asio::deadline_timer t1(io, boost::posix_time::seconds(1));
	t1.wait();

	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		slaves[i]->ChangeState(EtherCAT::Slave::State::OP);
	}
}

void Master::Teardown()
{
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		slaves[i]->ChangeState(EtherCAT::Slave::State::INIT, true);
	}

	manager->Stop();
}

void Master::DoHoming()
{
	boost::asio::io_service io;

	// Set the homing velocity
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
	{
		vel[i]->Velocity(HOMING_VELOCITY);
	}

	// Now keep monitoring the torque until they've all done
	int homeDoneNum = 0;
	bool homeDone[FLEXPICKER_SLAVES] = {false};

	while(homeDoneNum != FLEXPICKER_SLAVES)
	{
		for(int i = 0; i < FLEXPICKER_SLAVES; i++)
		{
			//std::cout << "T" << i << " " << vel[i]->Torque() << " ";
			if(vel[i]->Torque() > HOMING_TORQUE_HOME && !homeDone[i])
			{
				vel[i]->Velocity(0);
				homeDoneNum++;
				homeDone[i] = true;
			}
		}

		boost::asio::deadline_timer foo(io, boost::posix_time::milliseconds(1));
		foo.wait();
		//std::cout << std::endl;
	}

	// Now move down again for a time period.
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
		vel[i]->Velocity(-HOMING_VELOCITY * 10);
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
	t.wait();
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
		vel[i]->Velocity(0);

	boost::asio::deadline_timer t2(io, boost::posix_time::seconds(1));
	for(int i = 0; i < FLEXPICKER_SLAVES; i++)
		vel[i]->SetHome();
}

void Master::MoveTo(double x, double y, double z)
{
	// Run the kinematics!
	double theta[3];
	double xyz[3] = { x, y, z };
	double deltaTheta[3];
	double maxDelta = 0;
	double maxSpeed = 30.0;

	for(int i = 0; i < 3; i++)
		xyz[i] *= 1000; // Kinematics is in mm, we're in m

	inversekin(xyz, theta);

	// Multiply by -32
	for(int i = 0; i < 3; i++)
		theta[i] *= -32;

	// Scale the speeds by the amount to move
	for(int i = 0; i < 3; i++)
	{
		deltaTheta[i] = theta[i] - vel[i]->Position();
		if(deltaTheta[i] < 0)
			deltaTheta[i] = -deltaTheta[i];

		if(deltaTheta[i] > maxDelta)
			maxDelta = deltaTheta[i];
	}

	// Normalise. This is just dividing by the max value
	for(int i = 0; i < 3; i++)
		deltaTheta[i] /= maxDelta;

	for(int i = 0; i < 3; i++)
		vel[i]->PositionSetpoint(theta[i], (maxSpeed * deltaTheta[i]));

	std::cout << "Radians: x " << theta[0] << " y " << theta[1] << " z " << theta[2] << std::endl;
}

/*void Master::velocityControlThread()
{
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::milliseconds(INTERPOLATION_PERIOD_MS));

	while(runVelocityControlThread)
	{
		t.wait();
		t.expires_at(t.expires_at() + boost::posix_time::milliseconds(INTERPOLATION_PERIOD_MS));

		EtherCAT::Packet pkt;
		EtherCAT::Datagram::Pointer dgram[FLEXPICKER_SLAVES];

		for(int i = 0; i < FLEXPICKER_SLAVES; i++)
		{
			dgram[i] = vel[i]->GetDatagram();
			//dgram[i] = pos[i]->GetDatagram();
			pkt.AddDatagram(dgram[i]);
		}

		controlMux.lock();
		pkt.SendReceive(link);
		controlMux.unlock();

		for(int i = 0; i < FLEXPICKER_SLAVES; i++)
			vel[i]->UpdateData(dgram[i]);
		//pos[i]->UpdateData(dgram[i]);
		
	}
}*/
