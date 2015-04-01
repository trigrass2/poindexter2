#include "flexpicker_velocitycontroller.h"
#include "flexpicker_master.h"
#include "ax2000_params.h"
#include <canopen_sdo.h>
#include <datagram_address.h>

using namespace Flexpicker;

VelocityController::VelocityController(uint32_t fmmuAddr) : 
	fmmuAddr(fmmuAddr),
	control(0x06),
	status(0x0),
	position(0),
	velocity(0),
	velocityLimit(VELOCITY_LIMIT),
	done(true),
	positionSetpoint(0)
{

}

void VelocityController::SwitchOn()
{
	// Assert that we're in a condition where we can switch on
	if(status & CANOPEN_STATUS_FAULT_MASK)
		throw FlexPickerException("Attempted to switch on drive in fault mode");
	if(!(status & CANOPEN_STATUS_SWITCH_ON_MASK))
		throw FlexPickerException("Cannot switch on, ready to switch on not set");

	// Set the switch on bit in the control word!
	control |= CANOPEN_CONTROL_SWITCH_ON_MASK;
}

void VelocityController::EnableOperation()
{
	if(status & CANOPEN_STATUS_FAULT_MASK)
		throw FlexPickerException("Attempted to switch on drive in fault mode");
	if(!(status & CANOPEN_STATUS_SWITCHED_ON_MASK))
		throw FlexPickerException("Tried to enable; not switched on yet");

	control |= CANOPEN_CONTROL_ENABLE_OPERATION_MASK;
}

void VelocityController::SwitchOff()
{
	DisableOperation();
	control &= ~CANOPEN_CONTROL_SWITCH_ON_MASK;
}

void VelocityController::DisableOperation()
{
	control &= ~CANOPEN_CONTROL_ENABLE_OPERATION_MASK;
}

void VelocityController::Velocity(int32_t newVelocity)
{
	// TODO: Put a static check here to stop it going insane...
	velocity = newVelocity;

	done = true; // Stop interpolation taking place...
}

EtherCAT::Datagram::Pointer VelocityController::GetDatagram()
{
	EtherCAT::Datagram::Pointer dgram(new EtherCAT::DatagramLRW(AX2000_FMMU_IN_LENGTH, fmmuAddr));
	uint8_t* payload = dgram->payload_ptr();

	// Pack in the velocity and control word
	int32_t velocityUns = velocity;
	payload[0] = velocityUns & 0xFF;
	payload[1] = (velocityUns >> 8) & 0xFF;
	payload[2] = (velocityUns >> 16) & 0xFF;
	payload[3] = (velocityUns >> 24) & 0xFF;

	payload[4] = control & 0xFF;
	payload[5] = (control >> 8) & 0xFF;
	payload[6] = 0;
	payload[7] = 0; // Sanity...

	return dgram; 
}

void VelocityController::UpdateData(EtherCAT::Datagram::Pointer dgram)
{
	uint8_t* payload = dgram->payload_ptr();

	int32_t positionUns = 0;
	positionUns |= payload[0];
	positionUns |= (payload[1] << 8);
	positionUns |= (payload[2] << 16);
	positionUns |= (payload[3] << 24);

	int16_t newTorque = 0;
	newTorque |= (payload[4]);
	newTorque |= (payload[5] << 8);
	torque = newTorque;

	double positionDouble = positionUns;
	positionDouble *= POSITION_SCALER;
	position = positionDouble;

	uint16_t newStatus = 0;
	newStatus |= payload[6];
	newStatus |= (payload[7] << 8);
	status = newStatus;

	if(!done)
		doInterpolation();
}

void VelocityController::doInterpolation()
{
	// Proportional interpolator
	// Borrowed from the original sources
	// TODO: Fix acknowledgement	

	if(position > positionSetpoint - 0.1 &&
	   position < positionSetpoint + 0.1) 
	{
		done = true;
		velocity = 0;
		return;
	}

	double newVelocity = VELOCITY_KFB * (positionSetpoint - position) + VELOCITY_KFF * 0.0;

	if(newVelocity > velocityLimit)
		newVelocity = velocityLimit;
	if(newVelocity < -velocityLimit)
		newVelocity = -velocityLimit;

	// Scale
	newVelocity *= VELOCITY_SCALER;
	velocity = newVelocity;
}

void VelocityController::PositionSetpoint(double positionSetpoint, double maxVelocity)
{
	if(maxVelocity > VELOCITY_LIMIT)
		throw FlexPickerException("You can't move that fast!");

	// Justify it by the home location.
	this->positionSetpoint = positionSetpoint + homePosition;
	this->velocityLimit = maxVelocity;

	done = false;
}

void VelocityController::SetHome()
{
	homePosition = position;
}