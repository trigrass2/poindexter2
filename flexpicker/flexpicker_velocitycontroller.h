#ifndef __FLEXPICKER_VELOCITYCONTROLLER_H__
#define __FLEXPICKER_VELOCITYCONTROLLER_H__

#include <memory>
#include <boost/cstdint.hpp>
#include <datagram.h>

#define VELOCITY_KFB 25.0
#define VELOCITY_KFF 1.0
//#define VELOCITY_LIMIT 314.0
#define VELOCITY_LIMIT 300.0
#define VELOCITY_SCALER 1335.65
#define POSITION_SCALER (3.1415725 * 2.0 / (1048576.0))
#define MOTOR_RADS_TO_ARM -32.0

namespace Flexpicker
{

class VelocityController
{
public:
	typedef std::shared_ptr<VelocityController> Pointer;
	VelocityController(uint32_t fmmuAddr);

	// Get a datagram to write out the current values
	EtherCAT::Datagram::Pointer GetDatagram();

	// Notify it of new data to unpack
	// TODO: Should it just hold the datagram and unpack from there?
	// TODO: Can we do wait/notify on a datagram?
	void UpdateData(EtherCAT::Datagram::Pointer dgram);

	// Startup/shutdown
	void SwitchOn();
	void EnableOperation();

	void DisableOperation();
	void SwitchOff();

	int32_t Velocity() { return velocity; }
	double Position() { return position; }
	uint16_t Status()   { return status; }
	uint16_t Control()  { return control; }

	void Velocity(int32_t newVelocity);
	void PositionSetpoint(double positionSetpoint);

private:
	void doInterpolation();

	uint32_t fmmuAddr;

	int32_t velocity;
	double position;
	uint16_t status;
	uint16_t control;

	double positionSetpoint;

	bool done;
};

}

#endif