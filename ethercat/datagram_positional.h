#ifndef __DATAGRAM_POSITIONAL__
#define __DATAGRAM_POSITIONAL__

#include "datagram.h"

#define DGRAM_CMD_APRD 1
#define DGRAM_CMD_APWR 2
#define DGRAM_CMD_APRW 3

namespace EtherCAT
{

// These still aren't addressed, but have an offset for the data to read/write
class DatagramAPRD : public Datagram
{
public:
	DatagramAPRD(int payload_length, uint16_t slavePos, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_APRD;
		this->header->address |= (address << 16);
		this->header->address |= (-slavePos & 0xFFFF);
	}
};

class DatagramAPWR : public Datagram
{
public:
	DatagramAPWR(int payload_length, uint16_t slavePos, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_APWR;
		this->header->address |= (address << 16);
		this->header->address |= (-slavePos & 0xFFFF);
	}
};

class DatagramAPRW : public Datagram
{
public:
	DatagramAPRW(int payload_length, uint16_t slavePos, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_APRW;
		this->header->address |= (address << 16);
		this->header->address |= (-slavePos & 0xFFFF);
	}
};

}

#endif