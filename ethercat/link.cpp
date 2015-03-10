#include "link.h"

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include <boost/bind.hpp>

using namespace ethercat;

Link::Link(std::string iface, ReceivedCallback callback) : 
	recvStrand(io),
	ifName(iface), 
	recvCallback(callback),
	socket(io, RawProtocol(PF_PACKET, SOCK_RAW))
{
	// Create the socket and start attempting to listen
	// Borrowed from http://stackoverflow.com/questions/26212014/create-a-layer-2-ethernet-socket-with-boost-asio-raw-socket-in-c
	sockaddr_ll sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr_ll));
	sockaddr.sll_family = PF_PACKET;
	sockaddr.sll_protocol = htons(ETH_P_ALL);
	sockaddr.sll_ifindex = if_nametoindex(this->ifName.c_str());
	sockaddr.sll_hatype = 1;

	socket.bind(RawEndpoint(&sockaddr, sizeof(sockaddr_ll)));

	// Now start the async handlers and run!
	ethernetThread = boost::thread(boost::bind(&boost::asio::io_service::run, &this->io));
	socket.async_receive(boost::asio::buffer(this->recvBuffer), recvStrand.wrap(boost::bind(&Link::HandleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
}

Link::~Link()
{
	// Kill the IO handler to terminate the thread
	io.stop();

	// Wait for death
	ethernetThread.join();
}

void Link::SendData(PacketBuffer& data, int length)
{
	// Create the buffer object to send on...
	// This performs a copy so we can extend the lifetime of the object
	PacketBufferPointer buf(new PacketBuffer(data));
	SendData(buf, length);
}

void Link::SendData(PacketBufferPointer data, int length)
{
	socket.async_send(boost::asio::buffer(*data, length), boost::bind(&Link::HandleWrite, this, data));
}

void Link::HandleWrite(PacketBufferPointer data)
{
	// Nothing to do here...
	// This just takes an argument to extend the lifetime of the data to
	// be written and stop it from being destroyed.
}

void Link::HandleReceive(const boost::system::error_code& e, size_t bytes)
{
	recvCallback(this, recvBuffer, bytes);

	// Call the callback
	socket.async_receive(boost::asio::buffer(this->recvBuffer), recvStrand.wrap(boost::bind(&Link::HandleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
}