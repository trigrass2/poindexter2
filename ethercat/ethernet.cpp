/* This is a basic wrapper around L2 ethernet.
 * This just takes input data, adds on the MAC
 * layer, and forwards it on to the link layer
 */
 
#include "ethernet.h"
#include <boost/cstdlib.hpp>

using namespace ethercat;

Ethernet::Ethernet(std::string ifName, MACAddr macAddr, ReceivedCallback callback) : link(ifName, boost::bind(&Ethernet::NewPacket, this, _1, _2, _3)), recvCallback(callback)
{
	// boost::array doesn't expose a ctor for this
	this->macAddr = macAddr;
}

void Ethernet::NewPacket(Link* link, Link::PacketBuffer& packet, int length)
{
	// Strip out the guff and forward it on
	Ethernet::PacketBuffer ethPkt;
	unsigned char* ethData = ethPkt->data();
	unsigned char* linkData = packet->data();
	memcpy(ethData, linkData + ETHERNET_PREAMBLE_SIZE, length);

	recvCallback(this, ethPkt, length);
}

void Ethernet::SendData(PacketBuffer& packet, int length)
{
	// Create the packet and copy in the contents.
	Link::PacketBufferPointer sendPkt = Link::PacketBufferPointer(new Link::PacketBuffer());

	unsigned char* data = sendPkt->data();

	// Eh, not the best way...
	memset(data, 0xFF, ETHERNET_MAC_SIZE);
	memcpy(data+ETHERNET_MAC_SIZE, macAddr.data(), ETHERNET_MAC_SIZE);
	
	data[ETHERNET_MAC_SIZE*2+0] = 0x88;
	data[ETHERNET_MAC_SIZE*2+1] = 0xA4;

	memcpy(data + ETHERNET_PREAMBLE_SIZE, packet.data(), length);

	link.SendData(sendPkt, length + ETHERNET_PREAMBLE_SIZE);
}