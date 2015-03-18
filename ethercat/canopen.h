#ifndef __CANOPEN_H__
#define __CANOPEN_H__

#include "slave.h"
#include "syncmanager.h"
#include <memory>

namespace EtherCAT
{

class CANopen
{
public:
	typedef std::shared_ptr<CANopen> Pointer;

	CANopen(SyncManager::Pointer outMBox, SyncManager::Pointer inMBox);
	virtual ~CANopen();

	// SDO read/write
	// This is just for four-byte SDOs
	// I'm not implementing extended SDOs yet, since the AX2000
	// drives I'm testing on don't use them.
	void WriteSDO(uint16_t index, uint8_t subindex, uint32_t data);
	uint32_t ReadSDO(uint16_t index, uint8_t subindex);

private:
	SyncManager::Pointer outMBox;
	SyncManager::Pointer inMBox;
	
};

}

#endif
