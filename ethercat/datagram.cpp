#include "datagram.h"

#include <cstring>
#include <boost/cstdlib.hpp>

using namespace EtherCAT;

Datagram::Datagram(int payload_length)
{
	// Alloc the storage and set up the pointers
	this->storage = new unsigned char[payload_length + DGRAM_OVERHEADS];

	this->header = (DatagramHeader*)this->storage;
	this->payload = this->storage + DGRAM_HEADER_SIZE;
	this->wkc = (uint16_t*)(this->storage + DGRAM_HEADER_SIZE + payload_length);

	// Zero the storage
	memset(this->storage, 0, payload_length + DGRAM_OVERHEADS);

	this->header->lenRCM = payload_length & DGRAM_HDR_LEN_MASK;
}

Datagram::~Datagram()
{
	if(this->storage)
		delete [] this->storage;

	this->storage = NULL;
	this->header = NULL;
	this->payload = NULL;
	this->wkc = NULL;
}