#ifndef __CANOPEN_H__
#define __CANOPEN_H__

#include "slave.h"
#include "syncmanager.h"
#include <memory>

#include <exception>

#define COE_HEADER_SIZE 2
#define CANOPEN_MESSAGE_SIZE 8
#define CANOPEN_SDO_SIZE (COE_HEADER_SIZE + CANOPEN_MESSAGE_SIZE) // Two bytes of header, eight of payload.

#define COE_HEADER_TYPE_MASK 0xF000
#define COE_HEADER_TYPE_SDO_REQUEST  (2 << 12)
#define COE_HEADER_TYPE_SDO_RESPONSE (3 << 12)

#define SDO_COMMAND_MASK             0xE0
#define SDO_COMMAND_DOWNLOAD         (1 << 5)
#define SDO_COMMAND_DOWNLOAD_SEGMENT (0 << 5)
#define SDO_COMMAND_UPLOAD           (2 << 5)
#define SDO_COMMAND_UPLOAD_SEGMENT   (3 << 5)
#define SDO_COMMAND_ABORT            (4 << 5)

#define SDO_SIZE_MASK 0xC
#define SDO_SIZE_4    (0x0 << 2)
#define SDO_SIZE_3    (0x1 << 2)
#define SDO_SIZE_2    (0x2 << 2)
#define SDO_SIZE_1    (0x3 << 2)  

#define SDO_EXPEDITED_MASK 0x2
#define SDO_SIZE_SET_MASK  0x1

namespace EtherCAT
{

class CANopenException : public std::runtime_error
{
public:
	explicit CANopenException(const std::string& what) : std::runtime_error(what) { }
};

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
	// I'm aware of the wonky-ness of WriteSDO; it seemed a good idea at the time...
	void WriteSDO(uint16_t index, uint8_t subindex, uint32_t data, int data_size);
	uint32_t ReadSDO(uint16_t index, uint8_t subindex);

	// These assume they're operating on SyncManager 2 and 3 in buffered mode.
	// Override these from another class to change this for another drive.
	virtual void SetInputPDO(uint16_t inputPDO);
	virtual void SetOutputPDO(uint16_t outputPDO);

private:
	SyncManager::Pointer outMBox;
	SyncManager::Pointer inMBox;

};

}

#endif
