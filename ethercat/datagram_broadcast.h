#ifndef __DATAGRAM_BROADCAST__
#define __DATAGRAM_BROADCAST__

#include "datagram.h"

#define DGRAM_CMD_BRD 7
#define DGRAM_CMD_BWR 8
#define DGRAM_CMD_BRW 9

namespace EtherCAT
{

// These still aren't addressed, but have an offset for the data to read/write
class DatagramBRD : public Datagram
{
public:
	DatagramBRD(int payload_length, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_BRD;
		this->header->address |= (address << 16);
	}
};

class DatagramBWR : public Datagram
{
public:
	DatagramBWR(int payload_length, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_BWR;
		this->header->address |= (address << 16);
	}
};

class DatagramBRW : public Datagram
{
public:
	DatagramBRW(int payload_length, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_BRW;
		this->header->address |= (address << 16);
	}
};

}

#endif