#ifndef __FLEXPICKER_MASTER_H__
#define __FLEXPICKER_MASTER_H__

#include <link.h>
#include <slave.h>
#include <canopen.h>
#include "flexpicker_velocitycontroller.h"

#include <memory>
#include <boost/thread.hpp>

#define FLEXPICKER_SLAVES 3

namespace Flexpicker
{

class FlexPickerException : public std::runtime_error
{
public:
	explicit FlexPickerException(const std::string& what) : std::runtime_error(what) { }
};

class Master
{
public:
	typedef std::shared_ptr<Master> Pointer;
	Master(EtherCAT::Link::Pointer link);
	~Master();

	void Setup(); // Set up the EtherCAT stuff and go to OP state.
	void Teardown(); // Stop emitting outputs and fall back to INIT

	VelocityController::Pointer Controller(int index) { return vel[index]; }

private:
	EtherCAT::Link::Pointer link;
	EtherCAT::Slave::Pointer slaves[FLEXPICKER_SLAVES];
	EtherCAT::CANopen::Pointer slaveCan[FLEXPICKER_SLAVES];

	VelocityController::Pointer vel[FLEXPICKER_SLAVES];

	boost::thread controlThread;
	boost::mutex  controlMux;

	void velocityControlThread();
	volatile bool runVelocityControlThread;
};

}

#endif
