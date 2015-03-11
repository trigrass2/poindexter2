#include "link.h"
#include <iostream>


int main()
{
	std::cout << "Hello World!\n";
	unsigned char macAddr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

	EtherCAT::Link link("enp4s0", macAddr);
	EtherCAT::Link::EthernetPayload payload;

	// Make a dummy packet
	// The datagram has just the header and WKC, so is 12 bytes long
	// This has to be made up to 44 bytes due to padding though
	// Obviously, this is a bad test. This is a manually crafted
	// packet just to ensure that something works.
	unsigned char len = 44;
	payload[1] = 0x10 | ((len >> 8) & 0x07);
	payload[0] = len & 0xFF;

	// And zero the rest!
	for(int i = 3; i < len + 2; i++)
		payload[i] = 0;

	payload[2] = 0x7; // Broadcast read
	payload[8] = 0x1; // Length = 1

	// Transmit
	link.SendData(payload, 46);

	return 0;
}
