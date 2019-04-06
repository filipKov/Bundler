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

		uint GetHessianMulCamRowTask( 
			__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
			__in const uint cameraStartIx,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY,
			__in WorkerThread* pWorker, 
			__out ITask** ppTask )
		{
			uint taskSize = 0;
			const WorkerThreadInfo* pWorkerInfo = pWorker->GetInfo( );

			switch ( pWorkerInfo->type )
			{
			case WorkerThreadType::CPU:
				taskSize = GetHessianMulCamRowTaskCPU( pGlobalJacobian, cameraStartIx, diagonalDampeningFactor, vectorSize, pX, pY, ppTask );
				break;
			case WorkerThreadType::GPU:
			{
				const size_t acceleratorMemoryKB = ( ( WorkerThreadGPU* )pWorker )->GetGpuInfo( )->memoryKB;
				taskSize = GetHessianMulCamRowTaskGPU( pGlobalJacobian, cameraStartIx, diagonalDampeningFactor, vectorSize, pX, pY, acceleratorMemoryKB, ppTask );
				break;
			}
			default:
				break;
			}

			return taskSize;
		}

		uint GetHessianMulPointRowTask(
			__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
			__in const uint pointStartIx,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY,
			__in WorkerThread* pWorker,
			__out ITask** ppTask )
		{
			uint taskSize = 0;
			const WorkerThreadInfo* pWorkerInfo = pWorker->GetInfo( );

			switch ( pWorkerInfo->type )
			{
			case WorkerThreadType::CPU:
				taskSize = GetHessianMulPointRowTaskCPU( pGlobalJacobian, pointStartIx, diagonalDampeningFactor, vectorSize, pX, pY, ppTask );
				break;
			case WorkerThreadType::GPU:
			{
				const size_t acceleratorMemoryKB = ( ( WorkerThreadGPU* )pWorker )->GetGpuInfo( )->memoryKB;
				taskSize = GetHessianMulPointRowTaskGPU( pGlobalJacobian, pointStartIx, diagonalDampeningFactor, vectorSize, pX, pY, acceleratorMemoryKB, ppTask );
				break;
			}
			default:
				break;
			}

			return taskSize;
		}

	protected:

		uint GetHessianMulCamRowTaskCPU( 
			__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
			__in const uint cameraStartIx,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY,
			__out ITask** ppTask )
		{
			const uint cameraCount = min( m_maxCpuCamTaskSize, (uint)pGlobalJacobian->GetCameraCount( ) - cameraStartIx );

			HessianMulCamRowCPU< CameraModel >* pNewTask = new HessianMulCamRowCPU< CameraModel >( );
			pNewTask->Initialize( pGlobalJacobian, cameraStartIx, cameraCount, diagonalDampeningFactor, vectorSize, pX, pY );
			*ppTask = pNewTask;

			return cameraCount;
		}

		uint GetHessianMulCamRowTaskGPU(
			__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
			__in const uint cameraStartIx,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY,
			__in const size_t memoryLimitKB, 
			__out ITask** ppTask )
		{
			const uint cameraCount = min( m_maxCpuCamTaskSize, (uint)pGlobalJacobian->GetCameraCount( ) - cameraStartIx );

			UNREFERENCED_PARAMETER( memoryLimitKB ); // TODO: make GPU task!

			HessianMulCamRowCPU< CameraModel >* pNewTask = new HessianMulCamRowCPU< CameraModel >( );
			pNewTask->Initialize( pGlobalJacobian, cameraStartIx, cameraCount, diagonalDampeningFactor, vectorSize, pX, pY );
			*ppTask = pNewTask;

			return cameraCount;
		}

		uint GetHessianMulPointRowTaskCPU(
			__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
			__in const uint pointStartIx,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY,
			__out ITask** ppTask )
		{
			const uint pointCount = min( m_maxCpuPtTaskSize, ( uint )pGlobalJacobian->GetPointCount( ) - pointStartIx );

			HessianMulPointRowCPU< CameraModel >* pNewTask = new HessianMulPointRowCPU< CameraModel >( );
			pNewTask->Initialize( pGlobalJacobian, pointStartIx, pointCount, diagonalDampeningFactor, vectorSize, pX, pY );
			*ppTask = pNewTask;

			return pointCount;
		}

		uint GetHessianMulPointRowTaskGPU(
			__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
			__in const uint pointStartIx,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY,
			__in const size_t memoryLimitKB,
			__out ITask** ppTask )
		{
			UNREFERENCED_PARAMETER( memoryLimitKB ); // TODO: make GPU task!

			const uint pointCount = min( m_maxCpuPtTaskSize, ( uint )pGlobalJacobian->GetPointCount( ) - pointStartIx );

			HessianMulPointRowCPU< CameraModel >* pNewTask = new HessianMulPointRowCPU< CameraModel >( );
			pNewTask->Initialize( pGlobalJacobian, pointStartIx, pointCount, diagonalDampeningFactor, vectorSize, pX, pY );
			*ppTask = pNewTask;

			return pointCount;
		}

	protected:

		uint m_maxCpuCamTaskSize;
		uint m_maxGpuCamTaskSize;

		uint m_maxCpuPtTaskSize;
		uint m_maxGpuPtTaskSize;

	};

} } } }
