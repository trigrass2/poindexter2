#ifndef __FLEXPICKER_POSITIONCONTROLLER_H__
#define __FLEXPICKER_POSITIONCONTROLLER_H__

#include <memory>
#include <boost/cstdint.hpp>
#include <datagram.h>

namespace Flexpicker
{

class PositionController
{
public:
	typedef std::shared_ptr<PositionController> Pointer;
	PositionController(uint32_t fmmuAddr);

	EtherCAT::Datagram::Pointer GetDatagram();

	void UpdateData(EtherCAT::Datagram::Pointer dgram);

	void SwitchOn();
	void EnableOperation();
	void DisableOperation();
	void SwitchOff();

	uint32_t Position() { return position; }
	uint32_t PositionSetpoint() { return position_setpoint; }
	uint16_t Status() { return status; }
	uint16_t Control() { return control; }

	uint8_t OperationMode() { return operationModeActual; }

	void PositionSetpoint(uint32_t setpoint);
	void OperationMode(uint8_t operationMode);

private:
	uint32_t fmmuAddr;

	uint16_t control;
	uint16_t status;

	uint32_t position;
	uint32_t position_setpoint;

	uint8_t operationModeDemand;
	uint8_t operationModeActual;
};

}

#endif
