#include "stdafx.h"
#include "AMPUtils.h"
#include "ITask.h"
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

	void WorkerStack::ReturnWorker( WorkerThread* pWorker )
	{
		std::lock_guard< std::mutex > autoLock( m_stackLock );
		m_stack.Add( pWorker );
		m_stackPoll.notify_one( );
	}

	bool WorkerStack::HasAvailableWorker( )
	{
		return m_stack.Length( ) > 0;
	}

	template < WorkerThreadType type, typename ReturnWorkerCallback >
	void CreateWorkers( __in const uint count, __out_ecount( count ) WorkerThread** ppWorkers )
	{
		if ( type == WorkerThreadType::CPU )
		{
			for ( uint i = 0; i < count; i++ )
			{
				*ppWorkers = new WorkerThread< ReturnWorkerCallback >();
				ppWorkers++;
			}
		}
		else if ( type == WorkerThreadType::GPU )
		{
			Containers::Buffer< accelerator > accelerators;
			accelerators.Allocate( count );
			GetAccelerators( count, IsGPU );

			for ( uint i = 0; i < count; i++ )
			{
				*ppWorkers = new WorkerThreadGPU< ReturnWorkerCallback >( accelerators[i] );
				ppWorkers++;
			}
		}
	}

	WorkerPool::WorkerPool( )
	{
		const uint cpuWorkerCount = GetCpuThreadCount( );
		const uint gpuWorkerCount = GetAcceleratorCount( IsGPU );

		const uint workerCount = cpuWorkerCount + gpuWorkerCount;
		m_workerStack.Initialize( workerCount );
		m_workers.Allocate( workerCount );

		WorkerThread< ** ppWorkers = m_workers.Data( );
		
		auto returnWorkerCallback = std::bind( &WorkerStack::ReturnWorker, this->m_workerStack );
		CreateWorkers< WorkerThreadType::CPU >( &m_workerStack.ReturnWorker, cpuWorkerCount, ppWorkers );
		CreateWorkers< WorkerThreadType::GPU >( &m_workerStack.ReturnWorker, gpuWorkerCount, ppWorkers + cpuWorkerCount );

		for ( uint i = 0; i < workerCount; i++ )
		{
			(*ppWorkers)->
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

} }
