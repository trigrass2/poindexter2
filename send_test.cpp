#include <link.h>
#include <datagram.h>
#include <datagram_broadcast.h>
#include <packet.h>
#include <iostream>

int main()
{
	std::cout << "Hello World!\n";
	unsigned char macAddr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

	EtherCAT::Link link("enp4s0", macAddr);
	EtherCAT::Link::EthernetPayload payload;
	EtherCAT::Datagram::Pointer dgram(new EtherCAT::Datagram(50));
	EtherCAT::Datagram::Pointer dgram2(new EtherCAT::DatagramBRD(50, 0x8));
	EtherCAT::Packet pkt;

	pkt.AddDatagram(dgram);
	pkt.AddDatagram(dgram2);
	pkt.SendReceive(link);

	return 0;
}
