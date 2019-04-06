#pragma once

namespace Bundler { namespace Async { namespace LinearSolver {

	using CameraModel = CameraModels::CameraModel6DoF< 10 >;
	
	void MultiplyByHessian(
		__in const ProjectionProvider< CameraModel >* pGlobalJacobian,
		__in const Scalar diagonalDampeningFactor,
		__in const uint vectorSize,
		__in_ecount( vectorSize ) const Scalar* pX,
		__inout Async::WorkerPool* pWorkerPool,
		__out_ecount( vectorSize ) Scalar* pY )
	{
		const uint cameraCount = ( uint )pGlobalJacobian->GetCameraCount( );
		for ( uint cameraStartIx = 0; cameraStartIx < cameraCount; )
		{
			Async::WorkerThread* pWorker = pWorkerPool->GetWorker( );
		
			// TODO: create task
	
			pWorker->ExecuteTask( NULL );
	
			// Async::HessianCameraRowMultiplicationTask< CameraModel > task;
			// task.Initialize( 512000 );
			// 
			// task.InitializeWorkload( pJacobian, cameraStartIx, diagonalDampeningFactor, vectorSize, pX, pY );
			// 
			// cameraStartIx += task.GetJacobian( ).GetCameraCount( );
			// 
			// task.Execute( ); // TODO: execute task async
		}
	
		const uint pointCount = ( uint )pGlobalJacobian->GetPointCount( );
		for ( uint pointStartIx = 0; pointStartIx < pointCount; )
		{
			Async::WorkerThread* pWorker = pWorkerPool->GetWorker( );
	
			// TODO: create task
	
			pWorker->ExecuteTask( NULL );
	
			// Async::HessianPointRowMultiplicationTask< CameraModel > task;
			// task.Initialize( 512000 );
			// 
			// task.InitializeWorkload( pJacobian, pointStartIx, diagonalDampeningFactor, vectorSize, pX, pY );
			// 
			// pointStartIx += task.GetJacobian( ).GetPointCount( );
			// 
			// task.Execute( ); // TODO: execute task async
		}
	
		pWorkerPool->WaitForIdleWorkers( );
	}
	
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
