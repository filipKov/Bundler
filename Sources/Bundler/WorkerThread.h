#pragma once

namespace Bundler { namespace Async {

	class WorkerPool;

	class WorkerThread
	{
	public:

		WorkerThread( );

		~WorkerThread( );

		void SetReturnWorkerCallback ( IWorkerThreadCallback* pCallback );

		const WorkerThreadInfo* GetInfo( ) const;

		void ExecuteTask( __in ITask* pTask );

		WorkerThread( const WorkerThread& ) = delete;
		WorkerThread& operator = ( const WorkerThread& ) = delete;
	
	protected:

		void WorkerLoop( );

		void ReleaseTask( );

	protected:

		WorkerThreadInfo m_info;

		IWorkerThreadCallback* m_pReturnWorkerCallback;

		std::unique_ptr< std::thread > m_spExecutionThread;

		bool m_exit;
		
		ITask* m_pTaskToExecute;
		std::condition_variable m_taskPending;
		std::mutex m_taskLock;
	};

} }