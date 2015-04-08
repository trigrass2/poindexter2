#include "cyclic_communication_manager.h"

#include <functional>
#include <chrono>
#include <queue>
#include <iostream>

#include "packet.h"

using namespace EtherCAT;

CyclicCommunicationManager::CyclicCommunicationManager(Link::Pointer link, int periodMs) :
	link(link), periodMs(periodMs), runCyclicThread(false), acyclicReaderPtr(0), acyclicWriterPtr(0)
{

}

CyclicCommunicationManager::~CyclicCommunicationManager()
{
	Stop();
}

void CyclicCommunicationManager::RegisterCyclicController(CyclicController::Pointer controller)
{
	if(runCyclicThread)
		throw std::logic_error("Cannot add cyclic controllers to a running CyclicCommunicationManager");

	cyclicControllers.push_back(controller);
}

CyclicCommunicationManager::CyclicFuture CyclicCommunicationManager::SendDatagram(Datagram::Pointer dgram)
{
	// TODO: Add a check to this to assert there's actually space for it.
	// I'm missing it for now to test that it actually works...
	if(runCyclicThread)
		return SendDatagramCyclic(dgram);
	else
		return SendDatagramAcyclic(dgram);
}

CyclicCommunicationManager::CyclicFuture CyclicCommunicationManager::SendDatagramAcyclic(Datagram::Pointer dgram)
{
	// We're not currently running
	// Send it, then return an already fulfilled future.
	CyclicPromise p = CyclicPromise();
	CyclicFuture future = p.get_future();

	EtherCAT::Packet pkt;
	pkt.AddDatagram(dgram);
	pkt.SendReceive(link);

	p.set_value();

	return future;
}

CyclicCommunicationManager::CyclicFuture CyclicCommunicationManager::SendDatagramCyclic(Datagram::Pointer dgram)
{
	CyclicPromise p = CyclicPromise();
	CyclicFuture future = p.get_future();

	acyclicMessages[acyclicWriterPtr] = dgram;
	acyclicPromises[acyclicWriterPtr] = std::move(p);

	int newAcyclicWriterPtr = acyclicWriterPtr;
	newAcyclicWriterPtr++;
	newAcyclicWriterPtr %= ACYCLIC_REQUEST_MAX;
	acyclicWriterPtr = newAcyclicWriterPtr;

	return future;
}

void CyclicCommunicationManager::Run()
{
	if(runCyclicThread)
		return;

	runCyclicThread = true;
	cyclicThreadRef = std::thread(&CyclicCommunicationManager::cyclicThread, this);
}

void CyclicCommunicationManager::Stop()
{
	if(!runCyclicThread)
		return;
	
	runCyclicThread = false;
	cyclicThreadRef.join();
}

void CyclicCommunicationManager::cyclicThread()
{
	using std::chrono::system_clock;

	system_clock::time_point clk = system_clock::now();
	system_clock::duration addAmount = std::chrono::milliseconds(periodMs);

	clk += addAmount;

	while(runCyclicThread)
	{
		// Form the packet and fill it!
		std::queue<CyclicPromise> promises;

		EtherCAT::Packet pkt;

		// Add in the cyclic control information
		for(std::vector<CyclicController::Pointer>::iterator ctrl = cyclicControllers.begin();
			ctrl != cyclicControllers.end();
			ctrl++)
		{
			Datagram::Pointer dgram = (*ctrl)->GetDatagram();
			pkt.AddDatagram(dgram);
		}

		// And now anything in the queue...
		int wp = acyclicWriterPtr;
		int rp = acyclicReaderPtr;

		while(wp != rp)
		{
			pkt.AddDatagram(acyclicMessages[rp]);
			promises.push(std::move(acyclicPromises[rp]));
			rp++;
			rp %= ACYCLIC_REQUEST_MAX;
		}

		// Send it!
		pkt.SendReceive(link);

		// Notify the cyclic controllers
		for(std::vector<CyclicController::Pointer>::iterator ctrl = cyclicControllers.begin();
			ctrl != cyclicControllers.end();
			ctrl++)
		{
			(*ctrl)->UpdateData();
		}

		// Notify all of the promises
		while(!promises.empty())
		{
			promises.front().set_value();
			promises.pop();
		}

		// We're done, update the pointers
		acyclicReaderPtr = rp;

		std::this_thread::sleep_until(clk);
		clk += addAmount;
	}

	std::cout << "EXITING" << std::endl;
}