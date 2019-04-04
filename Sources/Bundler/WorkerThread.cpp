#include "stdafx.h"
#include "ITask.h"
#include "IWorkerThreadCallback.h"
#include "WorkerThreadDefs.h"
#include "WorkerThread.h"

namespace Bundler { namespace Async {

	WorkerThread::WorkerThread( )
	{
		m_info.type = WorkerThreadType::CPU;

		m_exit = false;
		m_spExecutionThread = std::unique_ptr< std::thread >( new std::thread( std::bind( &WorkerThread::WorkerLoop, this ) ) );
	}

	WorkerThread::~WorkerThread( )
	{
		{
			std::lock_guard< std::mutex > autoLock( m_taskLock );
			m_exit = true;
			m_taskPending.notify_one( );
			ReleaseTask( );
		}

		m_spExecutionThread->join( );
	}

	void WorkerThread::SetReturnWorkerCallback ( __in IWorkerThreadCallback* pCallback )
	{
		m_pReturnWorkerCallback = pCallback;
	}

	const WorkerThreadInfo* WorkerThread::GetInfo( ) const
	{
		return &m_info;
	}

	void WorkerThread::ExecuteTask( __in ITask* pTask )
	{
		std::lock_guard< std::mutex > autoLock( m_taskLock );
		m_pTaskToExecute = pTask;
		m_taskPending.notify_one( );
	}

	void WorkerThread::WorkerLoop( )
	{
		auto ShouldContinue = [ this ] ( ) -> bool
		{
			return m_exit || ( m_pTaskToExecute != NULL );
		};

		while ( true )
		{
			std::unique_lock< std::mutex > autoLock( m_taskLock );
			m_taskPending.wait( autoLock, ShouldContinue );

			if ( m_exit )
			{
				return;
			}

			m_pTaskToExecute->Execute( );
			ReleaseTask( );

			m_pReturnWorkerCallback->OnFinishTask( this );
		}
	}

	void WorkerThread::ReleaseTask( )
	{
		ITask* pTask = m_pTaskToExecute;
		m_pTaskToExecute = NULL;

		if ( pTask != NULL )
		{
			delete pTask;
		}
	}

} }
