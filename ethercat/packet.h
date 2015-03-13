#ifndef __PACKET_H__
#define __PACKET_H__

#include "link.h"
#include "datagram.h"

#define ETHERCAT_PACKET_HEADER_LENGTH 2
#define ETHERCAT_MINIMUM_PAYLOAD 44

namespace EtherCAT
{

class Packet
{
public:
	Packet();
	void AddDatagram(Datagram::Pointer& dgram);
	void SendReceive(Link::Pointer link);

private:
	Link::EthernetPayload payload;
	std::vector<Datagram::Pointer> datagrams;

	// This is badly named. This is actually the current payload size.
	int payloadPtr;
};

}

#endif