#include "link.h"

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include <boost/bind.hpp>

using namespace ethercat;

namespace ethercat
{
	// Implementation borrowed from the Boost::ASIO docs.
	class SendBuffer
	{
	public:
		explicit SendBuffer(const Link::PacketBuffer& data, int size) : data(data)
		{
			buf = boost::asio::buffer(this->data, size);
		}

	typedef boost::asio::const_buffer value_type;
	typedef const boost::asio::const_buffer* const_iterator;
	const boost::asio::const_buffer* begin() const { return &buf; }
	const boost::asio::const_buffer* end() const { return &buf + 1; }

	private:
		const Link::PacketBuffer data;
		boost::asio::const_buffer buf; 
	};
}

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

void Link::SendData(PacketBuffer data, int length)
{
	// Create the buffer object to send on...
	SendBuffer buf(data, length);
	socket.async_send(buf, boost::bind(&Link::HandleWrite, this));
}

void Link::HandleWrite()
{
	// Nothing to do here...
}

void Link::HandleReceive(const boost::system::error_code& e, size_t bytes)
{
	recvCallback(this, recvBuffer, bytes);

	// Call the callback
	socket.async_receive(boost::asio::buffer(this->recvBuffer), recvStrand.wrap(boost::bind(&Link::HandleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
}