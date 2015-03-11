#ifndef __PACKET_H__
#define __PACKET_H__

#include "link.h"
#include "datagram.h"

namespace EtherCAT
{

class Packet
{
public:
	Packet();
	void AddDatagram(Datagram::Pointer& dgram);
	void SendReceive(Link& link);

private:
	Link::EthernetPayload payload;
	std::vector<Datagram::Pointer> datagrams;
	int payloadPtr;
};

}

#endif