#pragma once

namespace Bundler { namespace LinearSolver { namespace Internal {

	template < class CameraModel >
	struct ParallelPCGInitSolveInitData
	{
		const ProjectionProvider< CameraModel >* pJacobian;
		Scalar diagonalDampeningFactor;

		uint parameterVectorSize;
		const Scalar* pParameterVector;

		ParallelPCGSolverTemp* pTemp;
	};

	template < class CameraModel >
	struct ParallelPCGLoopPart0InitData
	{
		const ProjectionProvider< CameraModel >* pJacobian;
		Scalar diagonalDampeningFactor;

		ParallelPCGSolverTemp* pTemp;
	};


	template < class CameraModel >
	struct ParallelPCGLoopPart1InitData
	{
		const ProjectionProvider< CameraModel >* pJacobian;

		Scalar alpha;
		uint parameterVectorSize;
		Scalar* pParameterVector;

		ParallelPCGSolverTemp* pTemp;
	};

	template < class CameraModel >
	class ParallelPCGTaskFactory
	{
	protected:

		static const uint m_maxCpuCamTaskSize = 32;
		static const uint m_maxCpuPtTaskSize = 8192 << 2;

		static const uint m_maxGpuCamTaskSize = 512;
		static const uint m_maxGpuPtTaskSize = 8192 << 3;

	public:

		static void CreateInitSolveCamerasTask(
			__in const Async::WorkerThread* pWorker,
			__in const ParallelPCGInitSolveInitData< CameraModel >* pInitData,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			Async::WorkerThreadType type = pWorker->GetInfo( )->type;

			switch ( type )
			{
			case Bundler::Async::WorkerThreadType::CPU:
				CreateInitSolveCamerasTaskCPU( pInitData, m_maxCpuCamTaskSize, pCameraStartIx, ppTask );
				break;
			case Bundler::Async::WorkerThreadType::GPU:
				CreateInitSolveCamerasTaskCPU( pInitData, m_maxCpuCamTaskSize, pCameraStartIx, ppTask ); // Fallback for non-compilable GPU init tasks
				break;
			default:
				*ppTask = NULL;
				break;
			}

		}

		static void CreateInitSolvePointsTask(
			__in const Async::WorkerThread* pWorker,
			__in const ParallelPCGInitSolveInitData< CameraModel >* pInitData,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			Async::WorkerThreadType type = pWorker->GetInfo( )->type;

			switch ( type )
			{
			case Bundler::Async::WorkerThreadType::CPU:
				CreateInitSolvePointsTaskCPU( pInitData, m_maxCpuPtTaskSize, pPointStartIx, ppTask );
				break;
			case Bundler::Async::WorkerThreadType::GPU:
				CreateInitSolvePointsTaskCPU( pInitData, m_maxCpuPtTaskSize, pPointStartIx, ppTask ); // Fallback for non-compilable GPU init tasks
				break;
			default:
				*ppTask = NULL;
				break;
			}
		}

		static void CreateLoopPart0CamerasTask(
			__in const Async::WorkerThread* pWorker,
			__in const ParallelPCGLoopPart0InitData< CameraModel >* pInitData,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			Async::WorkerThreadType type = pWorker->GetInfo( )->type;

			switch ( type )
			{
			case Bundler::Async::WorkerThreadType::CPU:
				// printf_s( "Creating CPU Loop 0 Camera Task\n" );
				CreateLoopPart0CamerasTaskCPU( pInitData, m_maxCpuCamTaskSize, pCameraStartIx, ppTask );
				break;
			case Bundler::Async::WorkerThreadType::GPU:
				// printf_s( "Creating GPU Loop 0 Camera Task\n" );
				CreateLoopPart0CamerasTaskAMP( pInitData, ((Async::WorkerThreadGPU*)pWorker)->GetGpuInfo(), pCameraStartIx, ppTask ); // TODO: temp
				break;
			default:
				*ppTask = NULL;
				break;
			}
		}

		static void CreateLoopPart0PointsTask(
			__in const Async::WorkerThread* pWorker,
			__in const ParallelPCGLoopPart0InitData< CameraModel >* pInitData,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			Async::WorkerThreadType type = pWorker->GetInfo( )->type;

			switch ( type )
			{
			case Bundler::Async::WorkerThreadType::CPU:
				// printf_s( "Creating CPU Loop 0 Points Task\n" );
				CreateLoopPart0PointsTaskCPU( pInitData, m_maxCpuPtTaskSize, pPointStartIx, ppTask );
				break;
			case Bundler::Async::WorkerThreadType::GPU:
				// printf_s( "Creating GPU Loop 0 Points Task\n" );
				CreateLoopPart0PointsTaskAMP( pInitData, ( ( Async::WorkerThreadGPU* )pWorker )->GetGpuInfo( ), pPointStartIx, ppTask ); // TODO: temp
				break;
			default:
				*ppTask = NULL;
				break;
			}
		}

		static void CreateLoopPart1CamerasTask(
			__in const Async::WorkerThread* pWorker,
			__in const ParallelPCGLoopPart1InitData< CameraModel >* pInitData,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			Async::WorkerThreadType type = pWorker->GetInfo( )->type;

			switch ( type )
			{
			case Bundler::Async::WorkerThreadType::CPU:
				// printf_s( "Creating CPU Loop 1 Camera Task\n" );
				CreateLoopPart1CamerasTaskCPU( pInitData, m_maxCpuCamTaskSize, pCameraStartIx, ppTask );
				break;
			case Bundler::Async::WorkerThreadType::GPU:
				// printf_s( "Creating GPU Loop 1 Camera Task\n" );
				CreateLoopPart1CamerasTaskAMP( pInitData, ( ( Async::WorkerThreadGPU* )pWorker )->GetGpuInfo( ), pCameraStartIx, ppTask ); // TODO: temp
				break;
			default:
				*ppTask = NULL;
				break;
			}
		}

		static void CreateLoopPart1PointsTask(
			__in const Async::WorkerThread* pWorker,
			__in const ParallelPCGLoopPart1InitData< CameraModel >* pInitData,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			Async::WorkerThreadType type = pWorker->GetInfo( )->type;

			switch ( type )
			{
			case Bundler::Async::WorkerThreadType::CPU:
				// printf_s( "Creating CPU Loop 1 Point Task\n" );
				CreateLoopPart1PointsTaskCPU( pInitData, m_maxCpuPtTaskSize, pPointStartIx, ppTask );
				break;
			case Bundler::Async::WorkerThreadType::GPU:
				// printf_s( "Creating GPU Loop 1 Point Task\n" );
				CreateLoopPart1PointsTaskAMP( pInitData, ( ( Async::WorkerThreadGPU* )pWorker )->GetGpuInfo( ), pPointStartIx, ppTask ); // TODO: temp
				break;
			default:
				*ppTask = NULL;
				break;
			}
		}

	protected:

		static uint GetTaskSize( 
			__in const uint elementStartIx, 
			__in const uint elementCount, 
			__in const uint maxTaskSize )
		{
			constexpr const float sizeTolerance = 0.05f;

			const uint remainingElements = elementCount - elementStartIx;

			if ( remainingElements > maxTaskSize )
			{
				const uint remainingAfterTask = remainingElements - maxTaskSize;
				if ( remainingAfterTask > uint( maxTaskSize * sizeTolerance ) )
				{
					return maxTaskSize;
				}
			}

			return remainingElements;
		}

		static void GetGPUCameraTaskSize(
			__in const uint cameraStartIx,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const int64 memoryKB,
			__out LocalProjectionProviderDataCountsAMP* pTaskSize )
		{
			//constexpr const float memoryFactor = 0.5f;
			//
			//const int64 jacobianMemoryLimit = int64( memoryKB * memoryFactor );
			//
			//LocalProjectionProviderDataAMP< CameraModel >::GetCountsForCameras( pJacobian, cameraStartIx, jacobianMemoryLimit, pTaskSize );
			//
			 uint taskSize = GetTaskSize( cameraStartIx, ( uint )pJacobian->GetCameraCount( ), m_maxGpuCamTaskSize );
			 LocalProjectionProviderDataAMP< CameraModel >::GetCountsForCameraBatch( pJacobian, cameraStartIx, taskSize, pTaskSize );
		}

		static void GetGPUPointTaskSize(
			__in const uint pointStartIx,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const int64 memoryKB,
			__out LocalProjectionProviderDataCountsAMP* pTaskSize )
		{
			// constexpr const float memoryFactor = 0.5f;
			// 
			// const int64 jacobianMemoryLimit = int64( memoryKB * memoryFactor );
			// 
			// LocalProjectionProviderDataAMP< CameraModel >::GetCountsForPoints( pJacobian, pointStartIx, jacobianMemoryLimit, pTaskSize );
			
			uint taskSize = GetTaskSize( pointStartIx, ( uint )pJacobian->GetPointCount( ), m_maxGpuPtTaskSize );
			LocalProjectionProviderDataAMP< CameraModel >::GetCountsForPointBatch( pJacobian, pointStartIx, taskSize, pTaskSize );
		}

		static void CreateInitSolveCamerasTaskCPU(
			__in const ParallelPCGInitSolveInitData< CameraModel >* pInitData,
			__in const uint maxTaskSize,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint cameraCount = ( uint )pInitData->pJacobian->GetCameraCount( );
			const uint cameraStartIx = *pCameraStartIx;
			const uint taskSize = GetTaskSize( cameraStartIx, cameraCount, maxTaskSize );

			PCGInitCamerasTaskCPU< CameraModel >* pTask = new PCGInitCamerasTaskCPU< CameraModel >();
			pTask->Initialize( 
				cameraStartIx, 
				taskSize, 
				pInitData->pJacobian, 
				pInitData->diagonalDampeningFactor, 
				pInitData->parameterVectorSize, 
				pInitData->pParameterVector, 
				pInitData->pTemp );

			( *pCameraStartIx ) += taskSize;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateInitSolvePointsTaskCPU(
			__in const ParallelPCGInitSolveInitData< CameraModel >* pInitData,
			__in const uint maxTaskSize,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint pointCount = ( uint )pInitData->pJacobian->GetPointCount( );
			const uint pointStartIx = *pPointStartIx;
			const uint taskSize = GetTaskSize( pointStartIx, pointCount, maxTaskSize );

			PCGInitPointsTaskCPU< CameraModel >* pTask = new PCGInitPointsTaskCPU< CameraModel >( );
			pTask->Initialize(
				pointStartIx,
				taskSize,
				pInitData->pJacobian,
				pInitData->diagonalDampeningFactor,
				pInitData->parameterVectorSize,
				pInitData->pParameterVector,
				pInitData->pTemp );

			( *pPointStartIx ) += taskSize;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart0CamerasTaskCPU(
			__in const ParallelPCGLoopPart0InitData< CameraModel >* pInitData,
			__in const uint maxTaskSize,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint cameraCount = ( uint )pInitData->pJacobian->GetCameraCount( );
			const uint cameraStartIx = *pCameraStartIx;
			const uint taskSize = GetTaskSize( cameraStartIx, cameraCount, maxTaskSize );

			PCGLoopPart0CamerasTaskCPU< CameraModel >* pTask = new PCGLoopPart0CamerasTaskCPU< CameraModel >( );
			pTask->Initialize(
				cameraStartIx,
				taskSize,
				pInitData->pJacobian,
				pInitData->diagonalDampeningFactor,
				pInitData->pTemp );

			( *pCameraStartIx ) += taskSize;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart0CamerasTaskAMP(
			__in const ParallelPCGLoopPart0InitData< CameraModel >* pInitData,
			__in const Async::GpuInfo* pAcceleratorInfo,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint cameraStartIx = *pCameraStartIx;

			LocalProjectionProviderDataCountsAMP taskSize = { 0 };
			GetGPUCameraTaskSize( cameraStartIx, pInitData->pJacobian, pAcceleratorInfo->memoryKB, &taskSize );

			accelerator_view acceleratorView = pAcceleratorInfo->accelerator.get_default_view( );

			PCGLoopPart0CamerasTaskAMP< CameraModel >* pTask = new PCGLoopPart0CamerasTaskAMP< CameraModel >( acceleratorView );
			pTask->Initialize(
				cameraStartIx,
				&taskSize,
				pInitData->pJacobian,
				pInitData->diagonalDampeningFactor,
				pInitData->pTemp );

			( *pCameraStartIx ) += taskSize.cameraCount;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart0PointsTaskCPU(
			__in const ParallelPCGLoopPart0InitData< CameraModel >* pInitData,
			__in const uint maxTaskSize,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint pointCount = ( uint )pInitData->pJacobian->GetPointCount( );
			const uint pointStartIx = *pPointStartIx;
			const uint taskSize = GetTaskSize( pointStartIx, pointCount, maxTaskSize );

			PCGLoopPart0PointsTaskCPU< CameraModel >* pTask = new PCGLoopPart0PointsTaskCPU< CameraModel >( );
			pTask->Initialize(
				pointStartIx,
				taskSize,
				pInitData->pJacobian,
				pInitData->diagonalDampeningFactor,
				pInitData->pTemp );

			( *pPointStartIx ) += taskSize;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart0PointsTaskAMP(
			__in const ParallelPCGLoopPart0InitData< CameraModel >* pInitData,
			__in const Async::GpuInfo* pAcceleratorInfo,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint pointStartIx = *pPointStartIx;

			LocalProjectionProviderDataCountsAMP taskSize = { 0 };
			GetGPUPointTaskSize( pointStartIx, pInitData->pJacobian, pAcceleratorInfo->memoryKB, &taskSize );

			accelerator_view acceleratorView = pAcceleratorInfo->accelerator.get_default_view( );

			PCGLoopPart0PointsTaskAMP< CameraModel >* pTask = new PCGLoopPart0PointsTaskAMP< CameraModel >( acceleratorView );
			pTask->Initialize(
				pointStartIx,
				&taskSize,
				pInitData->pJacobian,
				pInitData->diagonalDampeningFactor,
				pInitData->pTemp );

			( *pPointStartIx ) += taskSize.pointCount;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart1CamerasTaskCPU(
			__in const ParallelPCGLoopPart1InitData< CameraModel >* pInitData,
			__in const uint maxTaskSize,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint cameraCount = ( uint )pInitData->pJacobian->GetCameraCount( );
			const uint cameraStartIx = *pCameraStartIx;
			const uint taskSize = GetTaskSize( cameraStartIx, cameraCount, maxTaskSize );

			PCGLoopPart1CamerasTaskCPU< CameraModel >* pTask = new PCGLoopPart1CamerasTaskCPU< CameraModel >( );
			pTask->Initialize(
				cameraStartIx,
				taskSize,
				pInitData->pJacobian,
				pInitData->alpha,
				pInitData->parameterVectorSize,
				pInitData->pParameterVector,
				pInitData->pTemp );

			( *pCameraStartIx ) += taskSize;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart1CamerasTaskAMP(
			__in const ParallelPCGLoopPart1InitData< CameraModel >* pInitData,
			__in const Async::GpuInfo* pAcceleratorInfo,
			__inout uint* pCameraStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint cameraStartIx = *pCameraStartIx;

			LocalProjectionProviderDataCountsAMP taskSize = { 0 };
			GetGPUCameraTaskSize( cameraStartIx, pInitData->pJacobian, pAcceleratorInfo->memoryKB, &taskSize );

			accelerator_view acceleratorView = pAcceleratorInfo->accelerator.get_default_view( );

			PCGLoopPart1CamerasTaskAMP< CameraModel >* pTask = new PCGLoopPart1CamerasTaskAMP< CameraModel >( acceleratorView );
			pTask->Initialize(
				cameraStartIx,
				&taskSize,
				pInitData->pJacobian,
				pInitData->alpha,
				pInitData->parameterVectorSize,
				pInitData->pParameterVector,
				pInitData->pTemp );

			( *pCameraStartIx ) += taskSize.cameraCount;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart1PointsTaskCPU(
			__in const ParallelPCGLoopPart1InitData< CameraModel >* pInitData,
			__in const uint maxTaskSize,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint pointCount = ( uint )pInitData->pJacobian->GetPointCount( );
			const uint pointStartIx = *pPointStartIx;
			const uint taskSize = GetTaskSize( pointStartIx, pointCount, maxTaskSize );

			PCGLoopPart1PointsTaskCPU< CameraModel >* pTask = new PCGLoopPart1PointsTaskCPU< CameraModel >( );
			pTask->Initialize(
				pointStartIx,
				taskSize,
				pInitData->pJacobian,
				pInitData->alpha,
				pInitData->parameterVectorSize,
				pInitData->pParameterVector,
				pInitData->pTemp );

			( *pPointStartIx ) += taskSize;

			*ppTask = pTask;
			pTask = NULL;
		}

		static void CreateLoopPart1PointsTaskAMP(
			__in const ParallelPCGLoopPart1InitData< CameraModel >* pInitData,
			__in const Async::GpuInfo* pAcceleratorInfo,
			__inout uint* pPointStartIx,
			__deref_out Async::ITask** ppTask )
		{
			const uint pointStartIx = *pPointStartIx;

			LocalProjectionProviderDataCountsAMP taskSize = { 0 };
			GetGPUPointTaskSize( pointStartIx, pInitData->pJacobian, pAcceleratorInfo->memoryKB, &taskSize );

			accelerator_view acceleratorView = pAcceleratorInfo->accelerator.get_default_view( );

			PCGLoopPart1PointsTaskAMP< CameraModel >* pTask = new PCGLoopPart1PointsTaskAMP< CameraModel >( acceleratorView );
			pTask->Initialize(
				pointStartIx,
				&taskSize,
				pInitData->pJacobian,
				pInitData->alpha,
				pInitData->parameterVectorSize,
				pInitData->pParameterVector,
				pInitData->pTemp );

			( *pPointStartIx ) += taskSize.pointCount;

			*ppTask = pTask;
			pTask = NULL;
		}
		
	};

} } }
