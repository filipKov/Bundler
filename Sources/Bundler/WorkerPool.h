#pragma once

namespace Bundler { namespace Async {

	class WorkerStack
	{
	public:

		void Initialize( __in const uint workerCount );

		WorkerThread* GetWorker( );

		void ReturnWorker( WorkerThread* pWorker );

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

	protected:

		uint GetCpuThreadCount( );

	protected:

		WorkerStack m_workerStack;

		Containers::Buffer< WorkerThread* > m_workers;
	};

} }
