/* This is a wrapper around Boost ASIO to provide
 * raw socket functionality. It's quite basic, and because
 * it's asynchronous, provides read data back through a callback.
 * This contains its own io_service, and spawns a thread for it
 * to both send and receive data.
 */

#ifndef __LINK_H__
#define __LINK_H__

#include <queue>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#define LINK_MTU 1518

namespace ethercat
{

class Link
{
public:
	typedef boost::shared_ptr<Link> Pointer;
	typedef boost::array<unsigned char, LINK_MTU> PacketBuffer;
	typedef boost::shared_ptr<PacketBuffer> PacketBufferPointer;
	typedef boost::function3<void, Link*, PacketBuffer&, int> ReceivedCallback;
	Link(std::string iface, ReceivedCallback callback);
	~Link();

	void SendData(PacketBuffer& data, int length);
	void SendData(PacketBufferPointer data, int length);

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

	void HandleWrite(PacketBufferPointer data); // For when a write completes...
	void HandleReceive(const boost::system::error_code& e, size_t bytes);
};

};

#endif