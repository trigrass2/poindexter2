#ifndef __DATAGRAM_ADDRESS__
#define __DATAGRAM_ADDRESS__

#include "datagram.h"

#define DGRAM_CMD_LRD 10
#define DGRAM_CMD_LWR 11
#define DGRAM_CMD_LRW 12

namespace EtherCAT
{

// These still aren't addressed, but have an offset for the data to read/write
class DatagramLRD : public Datagram
{
public:
	DatagramLRD(int payload_length, uint32_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_LRD;
		this->header->address = address;
	}
};

class DatagramLWR : public Datagram
{
public:
	DatagramLWR(int payload_length, uint32_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_LWR;
		this->header->address = address;
	}
};

class DatagramLRW : public Datagram
{
public:
	DatagramLRW(int payload_length, uint32_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_LRW;
		this->header->address = address;
	}
};

}

#endif