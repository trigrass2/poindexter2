#include "link.h"

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

using namespace EtherCAT;

Link::Link(const std::string iface, const unsigned char macAddr[6]) : iface(iface), socket(io, RawProtocol(PF_PACKET, SOCK_RAW))
{
	// Create the socket and start attempting to listen
	// Borrowed from http://stackoverflow.com/questions/26212014/create-a-layer-2-ethernet-socket-with-boost-asio-raw-socket-in-c
	sockaddr_ll sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr_ll));
	sockaddr.sll_family = PF_PACKET;
	sockaddr.sll_protocol = htons(ETH_P_ALL);
	sockaddr.sll_ifindex = if_nametoindex(this->iface.c_str());
	sockaddr.sll_hatype = 1;

	socket.bind(RawEndpoint(&sockaddr, sizeof(sockaddr_ll)));

	// Now initialise the prolog
	// We assume sending to broadcast
	for(int i = 0; i < ETHERNET_MACADDR_SIZE; i++)
		prolog[i] = 0xFF;
	for(int i = ETHERNET_MACADDR_SIZE; i < ETHERNET_MACADDR_SIZE*2; i++)
		prolog[i] = macAddr[i - ETHERNET_MACADDR_SIZE];
	prolog[ETHERNET_MACADDR_SIZE*2 + 0] = (ETHERCAT_ETHERTYPE >> 8) & 0xFF;
	prolog[ETHERNET_MACADDR_SIZE*2 + 1] = ETHERCAT_ETHERTYPE & 0xFF;
}

int Link::SendData(const EthernetPayload& payload, int size)
{
	// Send the prolog and the payload
	boost::array<boost::asio::const_buffer, 2> packet = {
		boost::asio::buffer(this->prolog),
		boost::asio::buffer(payload, size)
	};

	return socket.send(packet) - ETHERNET_PROLOG;
}

int Link::SendData(const unsigned char* payload, int size)
{
	// Send the prolog and the payload
	boost::array<boost::asio::const_buffer, 2> packet = {
		boost::asio::buffer(this->prolog),
		boost::asio::buffer(payload, size)
	};

	return socket.send(packet) - ETHERNET_PROLOG;
}

int Link::ReceiveData(EthernetPayload& payload)
{
	// Temp buffer to read the header into
	EthernetProlog recvProlog;

	boost::array<boost::asio::mutable_buffer, 2> placeholder = {
		boost::asio::buffer(recvProlog),
		boost::asio::buffer(payload)
	};

	return socket.receive(placeholder) - ETHERNET_PROLOG;
}

int Link::ReceiveData(unsigned char* payload, int size)
{
	// Temp buffer to read the header into
	EthernetProlog recvProlog;

	boost::array<boost::asio::mutable_buffer, 2> placeholder = {
		boost::asio::buffer(recvProlog),
		boost::asio::buffer(payload, size)
	};

	return socket.receive(placeholder) - ETHERNET_PROLOG;
}