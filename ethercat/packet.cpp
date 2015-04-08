#include "packet.h"

#include <iostream>

using namespace EtherCAT;

Packet::Packet() : payloadPtr(0)
{

}

void Packet::AddDatagram(Datagram::Pointer& dgram)
{
	// Are there already datagrams there?
	if(datagrams.size())
		datagrams[datagrams.size() - 1]->M(1);

	datagrams.push_back(dgram);
	payloadPtr += dgram->datagram_length();
}

void Packet::SendReceive(Link::Pointer link)
{
	int dgram_pos = 0;

	// Set up the payload
	uint16_t packet_length = payloadPtr;

	// Is it too small? Round up if so.
	// OMG MAGIC NUMBERS!
	if(packet_length < ETHERCAT_MINIMUM_PAYLOAD)
		packet_length = ETHERCAT_MINIMUM_PAYLOAD;

	// Zero the payload
	memset(payload.data(), 0, packet_length);

	payload[0] = packet_length & 0xFF;       // Pack the length, little endian, 11 bits 
	payload[1] = (packet_length >> 8) & 0x7;
	payload[1] |= 0x10; // EtherCAT mode

	dgram_pos += 2;

	// Copy into the payload and go...
	for(std::vector<Datagram::Pointer>::iterator dgram = datagrams.begin();
		dgram != datagrams.end();
		dgram++)
	{
		// Get rid of the wrapper...
		Datagram::Pointer ptr = *dgram;

		memcpy(payload.data() + dgram_pos, ptr->data(), ptr->datagram_length());
		dgram_pos += ptr->datagram_length();
	}

	if(dgram_pos < (ETHERCAT_MINIMUM_PAYLOAD + ETHERCAT_PACKET_HEADER_LENGTH))
		dgram_pos = (ETHERCAT_MINIMUM_PAYLOAD + ETHERCAT_PACKET_HEADER_LENGTH);

	// Send it!
	link->SendData(payload, dgram_pos);
	
	// Receive it back again
	int length = link->ReceiveData(payload);
	if(length != dgram_pos)
		std::cerr << "WARNING: Length mismatch. Sent " << dgram_pos << " bytes, got " << length << " bytes." << std::endl;

	// Unpack that back into the datagrams.
	dgram_pos = 2;
	for(std::vector<Datagram::Pointer>::iterator dgram = datagrams.begin();
		dgram != datagrams.end();
		dgram++)
	{
		// Get rid of the wrapper...
		Datagram::Pointer ptr = *dgram;

		memcpy(ptr->data(), payload.data() + dgram_pos, ptr->datagram_length());
		dgram_pos += ptr->datagram_length();
	}

	// Nuke the contents of the packet
	datagrams.clear();
	payloadPtr = 0;
}