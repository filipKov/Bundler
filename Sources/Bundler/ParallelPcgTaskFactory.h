#pragma once

namespace Bundler { namespace Async { namespace LinearSolver { namespace Task {

	template < class CameraModel >
	class ParallelPcgTaskFactory
	{
	public:

		void Initialize( __in WorkerPool* pWorkerPool )
		{
			// const uint cpuWorkerCount = pWorkerPool->GetWorkerCount( WorkerThreadType::CPU );
			// const uint gpuWorkerCount = pWorkerPool->GetWorkerCount( WorkerThreadType::GPU );

			m_maxCpuCamTaskSize = 128;
			m_maxCpuPtTaskSize = 1024;

			m_maxGpuCamTaskSize = 128;
			m_maxGpuPtTaskSize = 1024;
		}

	protected:


	protected:

		uint m_maxCpuCamTaskSize;
		uint m_maxGpuCamTaskSize;

		uint m_maxCpuPtTaskSize;
		uint m_maxGpuPtTaskSize;

	};

} } } }
