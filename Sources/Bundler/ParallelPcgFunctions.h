#pragma once

namespace Bundler { namespace Async { namespace LinearSolver {

	// using CameraModel = CameraModels::CameraModel6DoF< 10 >;
	
	template < class CameraModel >
	void MultiplyByHessian(
		__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
		__in const Scalar diagonalDampeningFactor,
		__in const uint vectorSize,
		__in_ecount( vectorSize ) const Scalar* pX,
		__in Task::ParallelPcgTaskFactory< CameraModel >* pTaskFactory,
		__inout Async::WorkerPool* pWorkerPool,
		__out_ecount( vectorSize ) Scalar* pY )
	{
		const uint cameraCount = ( uint )pGlobalJacobian->GetCameraCount( );
		for ( uint cameraStartIx = 0; cameraStartIx < cameraCount; )
		{
			Async::WorkerThread* pWorker = pWorkerPool->GetWorker( );
		
			Async::ITask* pTask = NULL;
			cameraStartIx += pTaskFactory->GetHessianMulCamRowTask( pGlobalJacobian, cameraStartIx, diagonalDampeningFactor, vectorSize, pX, pY, pWorker, &pTask );
	
			pWorker->ExecuteTask( pTask );
		}
	
		const uint pointCount = ( uint )pGlobalJacobian->GetPointCount( );
		for ( uint pointStartIx = 0; pointStartIx < pointCount; )
		{
			Async::WorkerThread* pWorker = pWorkerPool->GetWorker( );

			Async::ITask* pTask = NULL;
			pointStartIx += pTaskFactory->GetHessianMulPointRowTask( pGlobalJacobian, pointStartIx, diagonalDampeningFactor, vectorSize, pX, pY, pWorker, &pTask );

			pWorker->ExecuteTask( pTask );
		}
	
		pWorkerPool->WaitForIdleWorkers( );
	}
	
	template < class CameraModel >
	void JtfNegSubtractX(
		__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
		__in const uint vectorSize,
		__inout_ecount( vectorSize ) Scalar* pX,
		__inout Async::WorkerPool* pWorkerPool )
	{
		const uint cameraCount = ( uint )pGlobalJacobian->GetCameraCount( );
		for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
		{
			Async::WorkerThread* pWorker = pWorkerPool->GetWorker( );
	
			// TODO: create task;
	
			pWorker->ExecuteTask( NULL );
		}
	
		pWorkerPool->WaitForIdleWorkers( );
	
		Vector< Scalar > x( vectorSize, pX );
		x.Negate( );
	}

} } }
