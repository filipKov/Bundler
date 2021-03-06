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
		m_stackCapacity = workerCount;
	}

	WorkerThread* WorkerStack::GetWorker( )
	{
		std::unique_lock< std::mutex > autoLock( m_stackLock );
		m_stackPoll.wait( autoLock, std::bind( &WorkerStack::HasAvailableWorker, this ) );
		
		return m_stack.RemoveLast( );
	}

	WorkerThread* WorkerStack::GetWorker( WorkerThreadType preferredType )
	{
		std::unique_lock< std::mutex > autoLock( m_stackLock );
		m_stackPoll.wait( autoLock, std::bind( &WorkerStack::HasAvailableWorker, this ) );

		for ( size_t desiredWorkerIx = 0; desiredWorkerIx < m_stack.Length( ); desiredWorkerIx++ )
		{
			if ( m_stack[desiredWorkerIx]->GetInfo( )->type == preferredType )
			{
				return m_stack.RemoveAt( desiredWorkerIx );
			}
		}

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

	void WorkerStack::WaitForIdleWorkers( )
	{
		std::unique_lock< std::mutex > autoLock( m_stackLock );
		m_stackPoll.wait( autoLock, std::bind( &WorkerStack::AllWorkersIdle, this ) );
	}

	bool WorkerStack::HasAvailableWorker( )
	{
		return m_stack.Length( ) > 0;
	}

	bool WorkerStack::AllWorkersIdle( )
	{
		return m_stack.Length( ) == m_stackCapacity;
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
		GetAccelerators( count, accelerators.Data( ), IsGPUWithEnoughMemory< 5000000 > );// IsGPU );

		for ( uint i = 0; i < count; i++ )
		{
			*ppWorkers = new WorkerThreadGPU( accelerators[i] );
			ppWorkers++;
		}
	}

	WorkerPool::WorkerPool( )
	{
		const uint cpuWorkerCount = GetCpuThreadCount( );
		const uint gpuWorkerCount = GetAcceleratorCount( IsGPUWithEnoughMemory< 5000000 > ); // IsGPU );

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

	WorkerThread* WorkerPool::GetWorker( )
	{
		return m_workerStack.GetWorker( );
	}

	WorkerThread* WorkerPool::GetWorker( WorkerThreadType preferredType )
	{
		return m_workerStack.GetWorker( preferredType );
	}

	void WorkerPool::WaitForIdleWorkers( )
	{
		m_workerStack.WaitForIdleWorkers( );
	}

	uint WorkerPool::GetWorkerCount( __in const WorkerThreadType type ) const
	{
		switch ( type )
		{
		case WorkerThreadType::CPU:
			return m_cpuWorkerCount;
		case WorkerThreadType::GPU:
			return m_gpuWorkerCount;
		default:
			return 0;
		}
	}

	uint WorkerPool::GetCpuThreadCount( )
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		return sysinfo.dwNumberOfProcessors;
	}

} }
