#include "ethernet.h"
#include <iostream>

void got_data(ethercat::Link* link, ethercat::Link::PacketBuffer& buf, int length)
{
	std::cout << "Got data!\n";
}

int main()
{
	std::cout << "Hello World!\n";

	ethercat::Ethernet::MACAddr macAddr = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	ethercat::Ethernet eth("enp4s0", macAddr);

	ethercat::Ethernet::PacketBuffer data;
	data[0] = 0xFF;

	eth.SendData(data, 1);

	for(volatile int i = 0; i < INT_MAX; i++);

	return 0;
}
