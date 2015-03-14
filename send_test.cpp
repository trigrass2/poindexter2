#include <link.h>
#include <slave.h>
#include <iostream>

#define EEPROM_READ_SIZE 4096

int main()
{
	//std::cout << "Hello World!\n";
	unsigned char macAddr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

	EtherCAT::Link::Pointer link(new EtherCAT::Link("enp4s0", macAddr));
	
	int nSlaves = EtherCAT::Slave::NumSlaves(link);
	//std::cout << "Found " << nSlaves << " slaves" << std::endl;

	EtherCAT::Slave::Pointer slaves[nSlaves];

	for(int i = 0; i < nSlaves; i++)
	{
		slaves[i] = EtherCAT::Slave::Pointer(new EtherCAT::Slave(link, i));
	}

	// Now read the EEPROM of the first one
	uint8_t buf[EEPROM_READ_SIZE];
	slaves[0]->ReadEEPROM(0, buf, EEPROM_READ_SIZE);

	for(int i = 0; i < EEPROM_READ_SIZE; i++)
	{
		std::cout << buf[i];
	}

	std::cout << std::endl << std::dec;

	return 0;
}
