#pragma once

namespace Bundler { namespace Async {

	struct GpuInfo
	{
		accelerator accelerator;
		int64 memoryKB;
	};

	class WorkerThreadGPU : public WorkerThread
	{
	public:
	
		WorkerThreadGPU( __in const accelerator gpu );
	
		const GpuInfo* GetGpuInfo( ) const;

		WorkerThreadGPU( const WorkerThreadGPU& ) = delete;
		WorkerThreadGPU& operator = ( const WorkerThreadGPU& ) = delete;

	protected:

		GpuInfo m_attachedGpuInfo;

	};

} }
