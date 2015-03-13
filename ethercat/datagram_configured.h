#ifndef __DATAGRAM_CFGADDR__
#define __DATAGRAM_CFGADDR__

#include "datagram.h"

#define DGRAM_CMD_FPRD 4
#define DGRAM_CMD_FPWR 5
#define DGRAM_CMD_FPRW 6

namespace EtherCAT
{

// These still aren't addressed, but have an offset for the data to read/write
class DatagramFPRD : public Datagram
{
public:
	DatagramFPRD(int payload_length, uint16_t slavePos, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_FPRD;
		this->header->address |= (address << 16);
		this->header->address |= (slavePos & 0xFFFF);
	}
};

class DatagramFPWR : public Datagram
{
public:
	DatagramFPWR(int payload_length, uint16_t slavePos, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_FPWR;
		this->header->address |= (address << 16);
		this->header->address |= (slavePos & 0xFFFF);
	}
};

class DatagramFPRW : public Datagram
{
public:
	DatagramFPRW(int payload_length, uint16_t slavePos, uint16_t address) : Datagram(payload_length)
	{
		this->header->cmd = DGRAM_CMD_FPRW;
		this->header->address |= (address << 16);
		this->header->address |= (slavePos & 0xFFFF);
	}
};

}

#endif