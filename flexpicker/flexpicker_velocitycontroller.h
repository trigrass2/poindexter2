#ifndef __FLEXPICKER_VELOCITYCONTROLLER_H__
#define __FLEXPICKER_VELOCITYCONTROLLER_H__

#include <memory>
#include <boost/cstdint.hpp>
#include <datagram.h>

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

	uint32_t Velocity() { return velocity; }
	uint32_t Position() { return position; }
	uint16_t Status()   { return status; }
	uint16_t Control()  { return control; }

	void Velocity(int32_t newVelocity);

private:
	uint32_t fmmuAddr;

	int32_t velocity;
	uint32_t position;
	uint16_t status;
	uint16_t control;
};

}

#endif