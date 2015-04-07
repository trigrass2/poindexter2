#ifndef __CYCLIC_COMMUNICATION_MANAGER_H__
#define __CYCLIC_COMMUNICATION_MANAGER_H__

#include <memory>
#include <future>
#include <vector>

#include <boost/lockfree/queue.hpp>

#include <thread>
#include <atomic>

#include "datagram.h"
#include "link.h"

#define ACYCLIC_REQUEST_MAX 10

namespace EtherCAT
{

// Base class so we can communicate with a Cyclic Controller setup
class CyclicController
{
public:
	typedef std::shared_ptr<CyclicController> Pointer;
	virtual ~CyclicController() = 0;

	// This function should return the datagram to be processed.
	// The parameter is provided by the CyclicCommunicationManager, and
	// is the future to wait on before the Datagram has valid results.
	// The returned Datagram MUST NOT be modified before this future notifies.
	// Having a void future doesn't feel like great design, but meh, it works.
	// We can fix it properly later...this needs to be patched onto the existing code :D
	virtual Datagram::Pointer GetDatagram(std::future<void> future) = 0;
};

class CyclicCommunicationManager
{
public:
	typedef std::shared_ptr<CyclicCommunicationManager> Pointer;
	typedef std::future<void> CyclicFuture;

	// link: The link to use for communcations.
	// periodMs: The period of communication, in ms. This must be large
	// enough to communicate with the cyclic communication managers.
	CyclicCommunicationManager(Link::Pointer link, int periodMs);
	~CyclicCommunicationManager();

	void RegisterCyclicController(CyclicController::Pointer controller);
	CyclicFuture SendDatagram(Datagram::Pointer dgram);

	void Run();
	void Stop();

private:
	typedef std::promise<void> CyclicPromise;

	Link::Pointer link;
	int periodMs;
	std::vector<CyclicController::Pointer> cyclicControllers;
	
	Datagram::Pointer acyclicMessages[ACYCLIC_REQUEST_MAX];
	CyclicPromise     acyclicPromises[ACYCLIC_REQUEST_MAX];

	volatile int acyclicReaderPtr;
	volatile int acyclicWriterPtr;

	std::thread cyclicThreadRef;
	bool runCyclicThread;
	void cyclicThread();
};

}

#endif
