#ifndef __FMMU_H__
#define __FMMU_H__

//#include "slave.h"
#include <memory>

namespace EtherCAT
{

class Slave;
typedef std::shared_ptr<Slave> SlavePointer;

class FMMU
{
public:
	typedef std::shared_ptr<FMMU> Pointer;
};

}

#endif
