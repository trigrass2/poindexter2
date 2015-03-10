#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include "link.h"

#include <boost/array.hpp>

#define ETHERNET_MTU 1500
#define ETHERNET_OVERHEADS 18
#define ETHERNET_MAC_SIZE 6
#define ETHERNET_ETHERTYPE_SIZE 2
#define ETHERNET_PREAMBLE_SIZE (ETHERNET_MAC_SIZE*2 + ETHERNET_ETHERTYPE_SIZE)

namespace ethercat
{

class Ethernet
{
public:
	// Ethernet payload packet.
	typedef boost::array<unsigned char, ETHERNET_MTU> PacketBuffer;
	typedef boost::array<unsigned char, ETHERNET_MAC_SIZE> MACAddr;
	typedef boost::function3<void, Ethernet*, PacketBuffer&, int> ReceivedCallback;

	// Create a new Link on this interface
	Ethernet(std::string ifName, MACAddr macAddr, ReceivedCallback callback); 

	// On packet received
	void NewPacket(Link* link, Link::PacketBuffer& packet, int length);
	void SendData(PacketBuffer& packet, int length);

private:
	Link link;
	MACAddr macAddr;
	ReceivedCallback recvCallback;
};

}

#endif