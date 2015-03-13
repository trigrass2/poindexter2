#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "link.h"
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <exception>
#include <string>

#define SLAVEADDR_BASE 0x10

namespace EtherCAT
{

class SlaveException : public std::runtime_error
{
public:
	explicit SlaveException(const std::string& what) : std::runtime_error(what) { }
};

class Slave
{
public:
	typedef boost::shared_ptr<Slave> Pointer;

	Slave(Link::Pointer link, uint16_t index);
	virtual ~Slave();

	// Static helpers
	static int NumSlaves(Link::Pointer link);
	static uint8_t SlaveType(Link::Pointer link, uint16_t slaveIdx);
	static uint8_t SlaveRev(Link::Pointer link, uint16_t slaveIdx);
	static uint16_t SlaveBuild(Link::Pointer link, uint16_t slaveIdx);
private:
	// Configured address variants
	static uint8_t readByteConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static uint16_t readShortConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static uint32_t readWordConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address);

	// Positional variants
	static uint8_t readBytePositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static uint16_t readShortPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static uint32_t readWordPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static void writeBytePositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint8_t data);
	static void writeShortPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint16_t data);
	static void writeWordPositional(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint32_t data);
	static int newSlaveAddr;
	Link::Pointer link;

	// To save requesting them every time
	uint8_t type;
	uint8_t revision;
	uint16_t build;
	uint8_t numFMMU;
	uint8_t numSyncManager;
	uint16_t slaveAddr;
};

}

#endif