#ifndef __LINK_H__
#define __LINK_H__

#include <queue>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#define ETHERNET_MTU 1518

namespace ethercat
{

class Link
{
public:
	typedef boost::array<unsigned char, ETHERNET_MTU> PacketBuffer;
	typedef boost::function3<void, Link*, PacketBuffer&, int> ReceivedCallback;
	Link(std::string iface, ReceivedCallback callback);
	~Link();

	void SendData(PacketBuffer data, int length);

private:
	typedef boost::asio::generic::raw_protocol RawProtocol;
	typedef boost::asio::generic::basic_endpoint<RawProtocol> RawEndpoint;

	boost::asio::io_service io;
	boost::asio::io_service::strand recvStrand; // To prevent races on inPktBuffer

	std::string ifName;

	RawProtocol::socket socket;
	PacketBuffer recvBuffer;
	ReceivedCallback recvCallback;

	boost::thread ethernetThread;

	void HandleWrite(); // For when a write completes...
	void HandleReceive(const boost::system::error_code& e, size_t bytes);
};

};

#endif