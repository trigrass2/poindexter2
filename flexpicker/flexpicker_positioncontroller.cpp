#include "flexpicker_positioncontroller.h"
#include "flexpicker_master.h"
#include "ax2000_params.h"
#include <canopen_sdo.h>
#include <datagram_address.h>

using namespace Flexpicker;

PositionController::PositionController(uint32_t fmmuAddr) :
	fmmuAddr(fmmuAddr),
	control(0x6),
	status(0),
	position(0),
	position_setpoint(0)
{

}

void PositionController::SwitchOn()
{
	// Assert that we're in a condition where we can switch on
	if(status & CANOPEN_STATUS_FAULT_MASK)
		throw FlexPickerException("Attempted to switch on drive in fault mode");
	if(!(status & CANOPEN_STATUS_SWITCH_ON_MASK))
		throw FlexPickerException("Cannot switch on, ready to switch on not set");

	// Set the switch on bit in the control word!
	control |= CANOPEN_CONTROL_SWITCH_ON_MASK;
}

void PositionController::EnableOperation()
{
	if(status & CANOPEN_STATUS_FAULT_MASK)
		throw FlexPickerException("Attempted to switch on drive in fault mode");
	if(!(status & CANOPEN_STATUS_SWITCHED_ON_MASK))
		throw FlexPickerException("Tried to enable; not switched on yet");

	control |= CANOPEN_CONTROL_ENABLE_OPERATION_MASK;
}

void PositionController::SwitchOff()
{
	DisableOperation();
	control &= ~CANOPEN_CONTROL_SWITCH_ON_MASK;
}

void PositionController::DisableOperation()
{
	control &= ~CANOPEN_CONTROL_ENABLE_OPERATION_MASK;
}

void PositionController::PositionSetpoint(uint32_t setpoint)
{
	// Check this properly later...
	position_setpoint = setpoint;

	control |= 0x1010;

	/*if(control & 0x10)
		control &= ~0x10;
	else
		control |= 0x10;*/
}

EtherCAT::Datagram::Pointer PositionController::GetDatagram()
{
	EtherCAT::Datagram::Pointer dgram(new EtherCAT::DatagramLRW(AX2000_FMMU_OUT_LENGTH, fmmuAddr));
	uint8_t* payload = dgram->payload_ptr();

	// Pack in the velocity and control word
	payload[0] = position_setpoint & 0xFF;
	payload[1] = (position_setpoint >> 8) & 0xFF;
	payload[2] = (position_setpoint >> 16) & 0xFF;
	payload[3] = (position_setpoint >> 24) & 0xFF;

	payload[4] = control & 0xFF;
	payload[5] = (control >> 8) & 0xFF;

	return dgram; 
}

void PositionController::UpdateData(EtherCAT::Datagram::Pointer dgram)
{
	uint8_t* payload = dgram->payload_ptr();

	uint32_t positionUns = 0;
	positionUns |= payload[0];
	positionUns |= (payload[1] << 8);
	positionUns |= (payload[2] << 16);
	positionUns |= (payload[3] << 24);

	position = positionUns;

	uint16_t newStatus = 0;
	newStatus |= payload[4];
	newStatus |= (payload[5] << 8);

	//if(newStatus & 0x1000)
	//	control &= ~0x10;
	if(newStatus & 0x8000)
		control &= ~0x1000;

	if(newStatus != status)
		std::cout << "STRANSITION 0x" << std::hex << status << " -> 0x" << newStatus << std::dec << std::endl;

	//f(newStatus & 0x3F00)
	///	std::cout << "STATUS: 0x" << std::hex << newStatus << std::dec << std::endl;

	status = newStatus;
}