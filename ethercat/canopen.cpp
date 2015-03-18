#include "canopen.h"

using namespace EtherCAT;

CANopen::CANopen(SyncManager::Pointer outMBox, SyncManager::Pointer inMBox) : 
	outMBox(outMBox), inMBox(inMBox)
{
}

CANopen::~CANopen()
{

}

void CANopen::WriteSDO(uint16_t index, uint8_t subindex, uint32_t data)
{

}

uint32_t CANopen::ReadSDO(uint16_t index, uint8_t subindex)
{
	return 0;
}
