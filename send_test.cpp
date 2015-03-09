#include "link.h"
#include <iostream>

void got_data(ethercat::Link* link, ethercat::Link::PacketBuffer& buf, int length)
{
	std::cout << "Got data!\n";
}

int main()
{
	std::cout << "Hello World!\n";

	ethercat::Link link("enp4s0", got_data);

	// Create a packet and send it!
	ethercat::Link::PacketBuffer buf;

	for(int i = 0; i < ETHERNET_MTU; i++)
		buf[i] = 0x0;

	buf[0] = 0xFF;
	buf[1] = 0xFF;
	buf[2] = 0xFF;
	buf[3] = 0xFF;
	buf[4] = 0xFF;
	buf[5] = 0xFF;

	buf[6] = 0x11;
	buf[7] = 0x22;
	buf[8] = 0x33;
	buf[9] = 0x44;
	buf[10] = 0x55;
	buf[11] = 0x66;

	buf[12] = 0x88;
	buf[13] = 0xA4;

	link.SendData(buf, 14);

	std::cout << "Waiting for death...\n";
	for(volatile int i = 0; i < INT_MAX; i++);
	std::cout << "Dying...\n";
	
	return 0;
}
