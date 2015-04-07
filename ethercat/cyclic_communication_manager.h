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
	virtual ~CyclicController() {}

	// This function should return the datagram to be sent in the cyclic controller.
	// This used to also accept a future. This does not accept a future any more.
	// The rationale is that actually using futures for this is complex; normally, the
	// CyclicController will wait on the future as soon as it is given. This will
	// block the CyclicCommunicationManager. The other way is for each CyclicController
	// to have its own thread. This complicates passing around the future and also, 
	// it is difficult to give each thread real-time priority. Moreover, updates of
	// a CyclicController will need to typically be synchronous on UpdateData anyway, and if
	// it takes too long to execute in UpdateData, it would end up missing deadlines anyway!
	// For this reason, the processing can be de-coupled if required, or just
	// run in the same thread on "UpdateData" if required.
	virtual Datagram::Pointer GetDatagram() = 0;
	virtual void UpdateData() = 0;
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
	CyclicFuture SendDatagramAcyclic(Datagram::Pointer dgram);
	CyclicFuture SendDatagramCyclic(Datagram::Pointer dgram);

	typedef std::promise<void> CyclicPromise;

	Link::Pointer link;
	int periodMs;
	std::vector<CyclicController::Pointer> cyclicControllers;
	
	Datagram::Pointer acyclicMessages[ACYCLIC_REQUEST_MAX];
	CyclicPromise     acyclicPromises[ACYCLIC_REQUEST_MAX];

	volatile int acyclicReaderPtr;
	volatile int acyclicWriterPtr;

	std::thread cyclicThreadRef;
	volatile bool runCyclicThread;
	void cyclicThread();
};

}

#endif
