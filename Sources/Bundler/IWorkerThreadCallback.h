#pragma once

namespace Bundler { namespace Async {

	class WorkerThread;

	class IWorkerThreadCallback
	{
	public:

		virtual void OnFinishTask( __in WorkerThread* pWorker ) = 0;

	};

} }