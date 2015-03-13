#ifndef __LINK_H__
#define __LINK_H__

#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#define ETHERNET_MTU 1500
#define ETHERNET_PROLOG 14
#define ETHERNET_MACADDR_SIZE 6
#define ETHERCAT_ETHERTYPE 0x88A4

namespace EtherCAT
{

class Link
{
public:
	typedef boost::array<unsigned char, ETHERNET_MTU> EthernetPayload;
	typedef boost::shared_ptr<Link> Pointer;

	Link(const std::string iface, const unsigned char macAddr[6]);

	// Send part of a given ethernet payload
	int SendData(const EthernetPayload& payload, int size);
	int SendData(const unsigned char* payload, int size);

	// Receive a packet
	int ReceiveData(EthernetPayload& payload);
	int ReceiveData(unsigned char* payload, int size);
private:
	typedef boost::asio::generic::raw_protocol RawProtocol;
	typedef boost::asio::generic::basic_endpoint<RawProtocol> RawEndpoint;
	typedef boost::array<unsigned char, ETHERNET_PROLOG> EthernetProlog;

	boost::asio::io_service io;
	RawProtocol::socket socket;
	EthernetProlog prolog;

	std::string iface;
};

}

#endif