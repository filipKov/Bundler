#pragma once

namespace Bundler { namespace Async {

	class WorkerStack : public IWorkerThreadCallback
	{
	public:

		void Initialize( __in const uint workerCount );

		WorkerThread* GetWorker( );

		void ReturnWorker( __in WorkerThread* pWorker );

		void OnFinishTask( __in WorkerThread* pWorker ) override;

	protected:

		bool HasAvailableWorker( );

	protected:

		std::condition_variable m_stackPoll;
		std::mutex m_stackLock;

		Containers::Vector< WorkerThread* > m_stack;

	};

	class WorkerPool
	{
	public:

		WorkerPool( );

		~WorkerPool( );

		WorkerThread* GetWorker( );

	protected:

		uint GetCpuThreadCount( );

	protected:

		WorkerStack m_workerStack;

		Containers::Buffer< WorkerThread* > m_workers;
	};

} }
