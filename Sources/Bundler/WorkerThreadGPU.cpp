#include "stdafx.h"
#include "ITask.h"
#include "IWorkerThreadCallback.h"
#include "WorkerThreadDefs.h"
#include "WorkerThread.h"
#include "WorkerThreadGPU.h"

namespace Bundler { namespace Async {

	WorkerThreadGPU::WorkerThreadGPU( __in const accelerator gpu ) : WorkerThread( )
	{
		m_info.type = WorkerThreadType::GPU;

		m_attachedGpuInfo.accelerator = gpu;
		m_attachedGpuInfo.memoryKB = int64( gpu.get_dedicated_memory( ) );
	}

	const GpuInfo* WorkerThreadGPU::GetGpuInfo( ) const
	{
		return &m_attachedGpuInfo;
	}

} }
