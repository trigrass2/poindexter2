#ifndef __DATAGRAM_H__
#define __DATAGRAM_H__

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#define DGRAM_HDR_LEN_MASK 0x7F
#define DGRAM_HDR_C_MASK 0x4000
#define DGRAM_HDR_C_SHIFT 14
#define DGRAM_HDR_M_MASK 0x8000
#define DGRAM_HDR_M_SHIFT 15

// 10 bytes for the header, two bytes for the WKC
#define DGRAM_HEADER_SIZE 10
#define DGRAM_OVERHEADS 12

namespace EtherCAT
{

// This isn't great, but will work for now
// NOTE: THIS WILL ONLY WORK ON LITTLE ENDIAN SYSTEMS.
// TODO: Make this also work on big-endian.
struct __attribute__((packed)) DatagramHeader
{
	uint8_t cmd;      // Command
	uint8_t idx;      // Packet index
	uint32_t address; // Command-defined address
	uint16_t lenRCM;  // Length/R/C/M
	uint16_t irq;     // Interrupt request
};

// This implements a NOP-packet
class Datagram
{
public:
	typedef boost::shared_ptr<Datagram> Pointer;
	Datagram(int payload_length);
	virtual ~Datagram();

	// Accessors
	uint8_t cmd()      { return header->cmd; }
	uint8_t idx()      { return header->idx; }
	uint32_t address() { return header->address; }
	uint16_t length()  { return header->lenRCM & DGRAM_HDR_LEN_MASK; }
	uint8_t C()        { return (header->lenRCM & DGRAM_HDR_C_MASK) >> DGRAM_HDR_C_SHIFT; }
	uint8_t M()        { return (header->lenRCM & DGRAM_HDR_M_MASK) >> DGRAM_HDR_M_SHIFT; }
	uint16_t working_counter() { return *this->wkc; }

	// Setters
	void idx(uint8_t x) { header->idx = x; }
	void M(uint8_t m)
	{
		header->lenRCM &= ~DGRAM_HDR_M_MASK;
		
		if(m)
			header->lenRCM |= DGRAM_HDR_M_MASK;
	}

	unsigned char* payload_ptr() { return payload; }
	unsigned char* data()    { return storage; }
	uint16_t datagram_length() { return length() + DGRAM_OVERHEADS; }

protected:
	unsigned char* storage;
	unsigned char* payload;
	DatagramHeader* header;
	uint16_t* wkc;
};

}

#endif