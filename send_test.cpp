#include <link.h>
#include <slave.h>
#include <iostream>

int main()
{
	std::cout << "Hello World!\n";
	unsigned char macAddr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

	EtherCAT::Link::Pointer link(new EtherCAT::Link("enp4s0", macAddr));
	
	int nSlaves = EtherCAT::Slave::NumSlaves(link);
	std::cout << "Found " << nSlaves << " slaves" << std::endl;

	EtherCAT::Slave::Pointer slaves[nSlaves];

	for(int i = 0; i < nSlaves; i++)
	{
		slaves[i] = EtherCAT::Slave::Pointer(new EtherCAT::Slave(link, i));
	}

	return 0;
}
