#ifndef __CANOPEN_H__
#define __CANOPEN_H__

#include "slave.h"
#include "syncmanager.h"
#include <memory>

#define COE_HEADER_SIZE 2
#define CANOPEN_MESSAGE_SIZE 8
#define CANOPEN_SDO_SIZE (COE_HEADER_SIZE + CANOPEN_MESSAGE_SIZE) // Two bytes of header, eight of payload.

#define COE_HEADER_TYPE_MASK 0xF000
#define COE_HEADER_TYPE_SDO_REQUEST  (2 << 12)
#define COE_HEADER_TYPE_SDO_RESPONSE (3 << 12)

#define SDO_COMMAND_MASK 0xE0
#define SDO_COMMAND_UPLOAD (2 << 5)

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
