#include "slave.h"

#include "slave_registers.h"
#include "packet.h"
#include "datagram_broadcast.h"
#include "datagram_positional.h"
#include "datagram_configured.h"

#include <iostream>
#include <stdexcept>

using namespace EtherCAT;

Slave::Slave(Link::Pointer link, uint16_t index) : link(link)
{
	// Read the parameters out for now
	// Might aswell use the statics for this :P
	type = SlaveType(link, index);
	revision = SlaveRev(link, index);
	build = SlaveBuild(link, index);
	numFMMU = readBytePositional(link, index, SLAVE_SUPPORTED_FMMU);
	numSyncManager = readBytePositional(link, index, SLAVE_SUPPORTED_SM);

	std::cerr << "Slave" 
		          << " type " << (int)type
		          << " rev " << (int)revision
		          << " build " << build
		          << " numFMMU " << (int)numFMMU
		          << " numSM " << (int)numSyncManager
		          << std::endl;

	// Remap
	writeShortPositional(link, index, SLAVE_STATION_ADDR, newSlaveAddr);
	slaveAddr = newSlaveAddr;
	newSlaveAddr++;

	// Assert it works
	// Read the address back again
	// This will throw an out of range exception if it fails
	try
	{
		uint16_t testAddr = readShortConfigured(link, slaveAddr, SLAVE_STATION_ADDR);
	}
	catch(std::out_of_range& ex)
	{
		throw SlaveException("Setting station address failed.");
	}
}

Slave::~Slave()
{

}

int Slave::NumSlaves(Link::Pointer link)
{
	// Send a broadcast packet and read back the WKC
	Datagram::Pointer datagram(new DatagramBRD(1, 0x0)); // Read one byte from the type register

	// Pack that into a packet and send it
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Read off the WKC
	return datagram->working_counter();
}

// The static helpers.
uint8_t Slave::SlaveType(Link::Pointer link, uint16_t slaveIdx)
{
	return readBytePositional(link, slaveIdx, SLAVE_TYPE);
}

uint8_t Slave::SlaveRev(Link::Pointer link, uint16_t slaveIdx)
{
	return readBytePositional(link, slaveIdx, SLAVE_REVISION);;
}

uint16_t Slave::SlaveBuild(Link::Pointer link, uint16_t slaveIdx)
{
	return readShortPositional(link, slaveIdx, SLAVE_BUILD);
}

// Private helpers
uint8_t Slave::readByteConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address)
{
	Datagram::Pointer datagram(new DatagramFPRD(1, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");

	return datagram->payload_ptr()[0];
}

uint16_t Slave::readShortConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address)
{
	Datagram::Pointer datagram(new DatagramFPRD(2, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");

	// Remember, ethercat is little-endian!
	uint16_t rv;
	rv = datagram->payload_ptr()[0];
	rv |= (datagram->payload_ptr()[1] << 8);

	return rv;
}

uint32_t Slave::readWordConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address)
{
	Datagram::Pointer datagram(new DatagramFPRD(4, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");

	// Remember, ethercat is little-endian!
	uint32_t rv;
	rv = datagram->payload_ptr()[0];
	rv |= (datagram->payload_ptr()[1] << 8);
	rv |= (datagram->payload_ptr()[2] << 16);
	rv |= (datagram->payload_ptr()[3] << 24);

	return rv;
}

uint8_t Slave::readBytePositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address)
{
	Datagram::Pointer datagram(new DatagramAPRD(1, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");

	return datagram->payload_ptr()[0];
}

uint16_t Slave::readShortPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address)
{
	Datagram::Pointer datagram(new DatagramAPRD(2, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");

	// Remember, ethercat is little-endian!
	uint16_t rv;
	rv = datagram->payload_ptr()[0];
	rv |= (datagram->payload_ptr()[1] << 8);

	return rv;
}

uint32_t Slave::readWordPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address)
{
	Datagram::Pointer datagram(new DatagramAPRD(4, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");

	// Remember, ethercat is little-endian!
	uint32_t rv;
	rv = datagram->payload_ptr()[0];
	rv |= (datagram->payload_ptr()[1] << 8);
	rv |= (datagram->payload_ptr()[2] << 16);
	rv |= (datagram->payload_ptr()[3] << 24);

	return rv;
}

void Slave::writeBytePositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint8_t data)
{
	Datagram::Pointer datagram(new DatagramAPWR(1, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	datagram->payload_ptr()[0] = data;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");
}

void Slave::writeShortPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint16_t data)
{
	Datagram::Pointer datagram(new DatagramAPWR(2, slaveIdx, address));

	// Return the first byte
	Packet pkt;

	// Unpack little-endian
	datagram->payload_ptr()[0] = data & 0xFF;
	datagram->payload_ptr()[1] = (data >> 8) & 0xFF;

	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");
}

void Slave::writeWordPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint32_t data)
{
	Datagram::Pointer datagram(new DatagramAPWR(4, slaveIdx, address));

	// Return the first byte
	Packet pkt;
	datagram->payload_ptr()[0] = data & 0xFF;
	datagram->payload_ptr()[1] = (data >> 8) & 0xFF;
	datagram->payload_ptr()[2] = (data >> 16) & 0xFF;
	datagram->payload_ptr()[3] = (data >> 24) & 0xFF;
	pkt.AddDatagram(datagram);
	pkt.SendReceive(link);

	// Check the working counter
	if(datagram->working_counter() == 0)
		throw std::out_of_range("slaveIdx");
}

int Slave::newSlaveAddr = SLAVEADDR_BASE;