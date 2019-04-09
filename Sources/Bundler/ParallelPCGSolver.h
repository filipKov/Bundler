#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class ParallelPCGSolver
	{
	public:
	
		void Initialize(
			__in const uint maxIterations,
			__in const Scalar errorTolerance,
			__in Async::WorkerPool* pWorkerPool )
		{
			m_maxIterations = maxIterations;
			m_errorToleranceSq = errorTolerance * errorTolerance;
			m_pWorkerPool = pWorkerPool;
		}
	
		void Initialize(
			__in const PCGSolverSettings& settings,
			__in Async::WorkerPool* pWorkerPool )
		{
			Initialize(
				settings.maxIterations,
				settings.errorTolerance,
				pWorkerPool );
		}
	
		void SolveSystem(
			__in const Scalar diagonalDampeningFactor,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_opt PCGSolverStatistics* pStatistics )
		{
			ParallelPCGSolverTemp temp( parameterVectorSize );
	
			InitSolve( pJacobian, diagonalDampeningFactor, parameterVectorSize, pX, &temp );
			uint finalIter = SolveLoop( pJacobian, diagonalDampeningFactor, parameterVectorSize, pX, &temp );
	
			if ( pStatistics )
			{
				pStatistics->iterationCount = finalIter;
				pStatistics->finalError = sqrt( temp.errSq );
			}
		}
	
	protected:
	
		void InitSolve(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out ParallelPCGSolverTemp* pTemp )
		{
			InitSolveCameras( pJacobian, diagonalDampeningFactor, parameterVectorSize, pX0, pTemp );
			InitSolvePoints( pJacobian, diagonalDampeningFactor, parameterVectorSize, pX0, pTemp );

			m_pWorkerPool->WaitForIdleWorkers( );
			pTemp->errSq = pTemp->errSqNew;
		}
	
		uint SolveLoop(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			Vector< Scalar > x( parameterVectorSize, pX );
	
			uint iteration = 0;
			while ( ( iteration < m_maxIterations ) &&
				( pTemp->errSq > m_errorToleranceSq ) )
			{
				pTemp->dDotAd = 0;
				LoopPart0Cameras( pJacobian, diagonalDampeningFactor, pTemp );
				LoopPart0Points( pJacobian, diagonalDampeningFactor, pTemp );

				m_pWorkerPool->WaitForIdleWorkers( ); // synchronize

				Scalar alpha = pTemp->errSq / dDotAd;

				pTemp->errSqNew = 0;
				LoopPart1Cameras( pJacobian, alpha, parameterVectorSize, pX, pTemp );
				LoopPart1Points( pJacobian, alpha, parameterVectorSize, pX, pTemp );

				m_pWorkerPool->WaitForIdleWorkers( ); // synchronize

				Scalar beta = pTemp->newErrSq / pTemp->errSq;
				pTemp->errSq = pTemp->newErrSq;

				// Not really a point in doing this in parallel ( probably ), maybe on CPU
				pTemp->d *= beta;
				pTemp->d += pTemp->MInvR;

				iteration++;
			}
	
			return iteration;
		}
	
	protected:

		// Performs initializations of PCG ( for camera part of the hessian ):
		// 0.1) r = A * x0;
		// 0.2) r = b - r;
		// 1) d = M^-1;
		// 2) errPart = r * d
		// 3) errSqNew += errPart( atomically )
		void InitSolveCameras(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out ParallelPCGSolverTemp* pTemp )
		{
			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
			for ( uint cameraStartIx = 0; cameraStartIx < cameraCount; )
			{
				Async::WorkerThread* pWorker = m_pWorkerPool->GetWorker( );

				Async::ITask* pTask = NULL;

				// TODO: createTask
				pWorker->ExecuteTask( pTask );
			}
		}

		// Performs initializations of PCG ( for point part of the hessian ):
		// 0.1) r = A * x0;
		// 0.2) r = b - r;
		// 1) d = M^-1;
		// 2) errPart = r * d
		// 3) errSqNew += errPart( atomically )
		void InitSolvePoints(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out ParallelPCGSolverTemp* pTemp )
		{
			const uint pointCount = ( uint )pJacobian->GetPointCount( );
			for ( uint pointStartIx = 0; pointStartIx < pointCount; )
			{
				Async::WorkerThread* pWorker = m_pWorkerPool->GetWorker( );

				Async::ITask* pTask = NULL;

				// TODO: createTask
				pWorker->ExecuteTask( pTask );
			}
		}

		// Perfoms following operations of PCG Loop ( for camera part of the hessian ):
		// 0) Ad = A * d;
		// 1) partDot = d * Ad;
		// 2) dot += partDot (atomically)
		void LoopPart0Cameras( 
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
			for ( uint cameraStartIx = 0; cameraStartIx < cameraCount; )
			{
				Async::WorkerThread* pWorker = m_pWorkerPool->GetWorker( );
				
				Async::ITask* pTask = NULL;

				// TODO: createTask
				pWorker->ExecuteTask( pTask );
			}
		}

		// Perfoms following operations of PCG Loop ( for point part of the hessian ):
		// 0) Ad = A * d;
		// 1) partDot = d * Ad;
		// 2) dot += partDot (atomically)
		void LoopPart0Points(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			const uint pointCount = ( uint )pJacobian->GetPointCount( );
			for ( uint pointStartIx = 0; pointStartIx < pointCount; )
			{
				Async::WorkerThread* pWorker = m_pWorkerPool->GetWorker( );

				Async::ITask* pTask = NULL;

				// TODO: createTask
				pWorker->ExecuteTask( pTask );
			}
		}

		// Perfoms following operations of PCG Loop ( for camera part of the hessian ):
		// 0) x += alpha * d;
		// 1) r -= alpha * Ad;
		// 2) mr = M^-1 * r;
		// 3) errNewPart = r * mr
		// 4) errNew += errNewPart( atomically )
		void LoopPart1Cameras(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar alpha,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
			for ( uint cameraStartIx = 0; cameraStartIx < cameraCount; )
			{
				Async::WorkerThread* pWorker = m_pWorkerPool->GetWorker( );

				Async::ITask* pTask = NULL;

				// TODO: createTask
				pWorker->ExecuteTask( pTask );
			}
		}

		// Perfoms following operations of PCG Loop ( for point part of the hessian ):
		// 0) x += alpha * d;
		// 1) r -= alpha * Ad;
		// 2) mr = M^-1 * r;
		// 3) errNewPart = r * mr
		// 4) errNew += errNewPart( atomically )
		void LoopPart1Points(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar alpha,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			const uint pointCount = ( uint )pJacobian->GetPointCount( );
			for ( uint pointStartIx = 0; pointStartIx < pointCount; )
			{
				Async::WorkerThread* pWorker = m_pWorkerPool->GetWorker( );

				Async::ITask* pTask = NULL;

				// TODO: createTask
				pWorker->ExecuteTask( pTask );
			}
		}
	
	protected:
	
		uint m_maxIterations;
		Scalar m_errorToleranceSq;
	
		Async::WorkerPool* m_pWorkerPool;
	
	};

} }
