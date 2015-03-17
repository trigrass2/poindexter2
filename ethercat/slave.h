#ifndef __SLAVE_H__
#define __SLAVE_H__

#include "link.h"
#include "fmmu.h"
#include "syncmanager.h"
#include <boost/cstdint.hpp>
#include <memory>

#include <exception>
#include <string>
#include <vector>

#define SLAVEADDR_BASE 0x10

namespace EtherCAT
{

class SlaveException : public std::runtime_error
{
public:
	explicit SlaveException(const std::string& what) : std::runtime_error(what) { }
};

class Slave : public std::enable_shared_from_this<Slave>
{
public:
	enum class State
	{
		INIT,
		PREOP,
		SAFEOP,
		OP,
		BOOT
	};

	typedef std::shared_ptr<Slave> Pointer;

	Slave(Link::Pointer link, uint16_t index);
	virtual ~Slave();

	virtual void Init();

	// Read the EEPROM
	// This provides a default, dumb implementation.
	// Tailor this for the actual Slave ESC being used
	virtual void ReadEEPROM(uint32_t address, uint8_t* data, int count);

	uint8_t NumFMMUs() { return numFMMU; }
	uint8_t NumSyncManager() { return numSyncManager; }
	uint16_t SlaveAddress() { return slaveAddr; }

	// Access memory space.
	// It is recommended to not use there where possible...
	uint8_t ReadByte(uint16_t address)    { return readByteConfigured (link, slaveAddr, address); }
	uint16_t ReadShort(uint16_t address)  { return readShortConfigured(link, slaveAddr, address); }
	uint32_t ReadWord(uint16_t address)   { return readWordConfigured (link, slaveAddr, address); }

	void WriteByte(uint16_t address, uint8_t data)   { writeByteConfigured(link, slaveAddr, address, data); }
	void WriteShort(uint16_t address, uint16_t data) { writeShortConfigured(link, slaveAddr, address, data); }
	void WriteWord(uint16_t address, uint32_t data)  { writeWordConfigured(link, slaveAddr, address, data); }

	void ReadData(uint16_t address, uint8_t* data, uint16_t length);
	void WriteData(uint16_t address, uint8_t* data, uint16_t length);

	void ChangeState(State newState);

	// These are this way by convention.
	// Override in a derived class if required
	virtual SyncManager::Pointer SyncManagerOutMBox() { return syncManagers[0]; }
	virtual SyncManager::Pointer SyncManagerInMBox()  { return syncManagers[1]; }
	virtual SyncManager::Pointer SyncManagerOutPDO()  { return syncManagers[2]; }
	virtual SyncManager::Pointer SyncManagerInPDO()   { return syncManagers[3]; }

	// Static helpers
	static int NumSlaves(Link::Pointer link);
	static uint8_t SlaveType(Link::Pointer link, uint16_t slaveIdx);
	static uint8_t SlaveRev(Link::Pointer link, uint16_t slaveIdx);
	static uint16_t SlaveBuild(Link::Pointer link, uint16_t slaveIdx);
private:
	void awaitALChange();

	// Configured address variants
	static uint8_t readByteConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static uint16_t readShortConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static uint32_t readWordConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address);
	static void writeByteConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint8_t data);
	static void writeShortConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint16_t data);
	static void writeWordConfigured(Link::Pointer link, uint16_t slaveIdx, uint16_t address, uint32_t data);

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

	State state;

	std::vector<SyncManager::Pointer> syncManagers;
	std::vector<FMMU::Pointer> FMMUs;
};

}

#endif