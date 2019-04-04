#include "stdafx.h"
#include "AMPUtils.h"
#include "ITask.h"
#include "IWorkerThreadCallback.h"
#include "WorkerThreadDefs.h"
#include "WorkerThread.h"
#include "WorkerThreadGPU.h"
#include "WorkerPool.h"

namespace Bundler { namespace Async {

	void WorkerStack::Initialize( __in const uint workerCount )
	{
		m_stack.EnsureCapacity( workerCount );
	}

	WorkerThread* WorkerStack::GetWorker( )
	{
		std::unique_lock< std::mutex > autoLock( m_stackLock );
		m_stackPoll.wait( autoLock, std::bind( &WorkerStack::HasAvailableWorker, this ) );
		
		return m_stack.RemoveLast( );
	}

	void WorkerStack::ReturnWorker( __in WorkerThread* pWorker )
	{
		std::lock_guard< std::mutex > autoLock( m_stackLock );
		m_stack.Add( pWorker );
		m_stackPoll.notify_one( );
	}

	void WorkerStack::OnFinishTask( __in WorkerThread* pWorker )
	{
		ReturnWorker( pWorker );
	}

	bool WorkerStack::HasAvailableWorker( )
	{
		return m_stack.Length( ) > 0;
	}


	template < WorkerThreadType type >
	void CreateWorkers( __in const uint count, __out_ecount( count ) WorkerThread** ppWorkers );

	template <>
	void CreateWorkers< WorkerThreadType::CPU >( __in const uint count, __out_ecount( count ) WorkerThread** ppWorkers )
	{
		for ( uint i = 0; i < count; i++ )
		{
			*ppWorkers = new WorkerThread( );
			ppWorkers++;
		}
	}

	template <>
	void CreateWorkers< WorkerThreadType::GPU >( __in const uint count, __out_ecount( count ) WorkerThread** ppWorkers )
	{
		Containers::Buffer< accelerator > accelerators;
		accelerators.Allocate( count );
		GetAccelerators( count, accelerators.Data(), IsGPU );

		for ( uint i = 0; i < count; i++ )
		{
			*ppWorkers = new WorkerThreadGPU( accelerators[i] );
			ppWorkers++;
		}
	}

	WorkerPool::WorkerPool( )
	{
		const uint cpuWorkerCount = GetCpuThreadCount( );
		const uint gpuWorkerCount = GetAcceleratorCount( IsGPU );

		const uint workerCount = cpuWorkerCount + gpuWorkerCount;
		m_workerStack.Initialize( workerCount );
		m_workers.Allocate( workerCount );

		WorkerThread** ppWorkers = m_workers.Data( );
		CreateWorkers< WorkerThreadType::CPU >( cpuWorkerCount, ppWorkers );
		CreateWorkers< WorkerThreadType::GPU >( gpuWorkerCount, ppWorkers + cpuWorkerCount );

		for ( uint i = 0; i < workerCount; i++ )
		{
			( *ppWorkers )->SetReturnWorkerCallback( &m_workerStack );
			m_workerStack.ReturnWorker( *ppWorkers );
			ppWorkers++;
		}
	}
	
	
	WorkerPool::~WorkerPool( )
	{
		WorkerThread** ppWorkers = m_workers.Data( );
		for ( size_t i = 0; i < m_workers.Length( ); i++ )
		{
			WorkerThread* pWorker = *ppWorkers;
			*ppWorkers = NULL;
			
			if ( pWorker )
			{
				MemoryAllocator< WorkerThread >::ReleaseItem( pWorker );
			}
		}
	}

	uint WorkerPool::GetCpuThreadCount( )
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		return sysinfo.dwNumberOfProcessors;
	}

	WorkerThread* WorkerPool::GetWorker( )
	{
		return m_workerStack.GetWorker( );
	}

} }
