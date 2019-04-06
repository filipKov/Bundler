#pragma once

namespace Bundler { namespace Async { namespace LinearSolver {

	using PCGSolverSettings = Bundler::LinearSolver::PCGSolverSettings;
	using PCGSolverStatistics = Bundler::LinearSolver::PCGSolverStatistics;
	using PCGSolverTemp = Bundler::LinearSolver::PCGSolverTemp;


	template < class CameraModel >
	class ParallelPCGSolver
	{
	public:
	
		void Initialize(
			__in const uint maxIterations,
			__in const Scalar errorTolerance,
			// __in const Preconditioner< CameraModel >* pPreconditioner,
			__in Async::WorkerPool* pWorkerPool )
		{
			m_maxIterations = maxIterations;
			m_errorToleranceSq = errorTolerance * errorTolerance;
			// m_pPreconditioner = pPreconditioner;
			m_pWorkerPool = pWorkerPool;
		}
	
		void Initialize(
			__in const PCGSolverSettings& settings,
			// __in const Preconditioner< CameraModel >* pPreconditioner,
			__in Async::WorkerPool* pWorkerPool )
		{
			Initialize(
				settings.maxIterations,
				settings.errorTolerance,
				// pPreconditioner,
				pWorkerPool );
		}
	
		void SolveSystem(
			__in const Scalar diagonalDampeningFactor,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_opt PCGSolverStatistics* pStatistics )
		{
			PCGSolverTemp temp( parameterVectorSize );
	
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
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out PCGSolverTemp* pTemp )
		{
			// parallel
			GetInitialResidual( pJacobian, diagonalDampeningFactor, parameterVectorSize, pX, pTemp->r.Elements( ) );
			ApplyPreconditioner( pJacobian, parameterVectorSize, pTemp->r.Elements( ), pTemp->d.Elements( ) );
	
			// sync here
	
			// seq
			pTemp->errSq = pTemp->r.Dot( pTemp->d );
		}
	
		uint SolveLoop(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout PCGSolverTemp* pTemp )
		{
			Vector< Scalar > x( parameterVectorSize, pX );
	
			uint iteration = 0;
			while ( ( iteration < m_maxIterations ) &&
				( pTemp->errSq > m_errorToleranceSq ) )
			{
				MultiplyByHessian( pJacobian, diagonalDampeningFactor, parameterVectorSize, pTemp->d.Elements( ), pTemp->Ad.Elements( ) );
	
				Scalar alpha = pTemp->errSq / ( pTemp->d.Dot( pTemp->Ad ) );
				x += alpha * pTemp->d;
				pTemp->r -= alpha * pTemp->Ad; // maybe do "r = b - Ax" every 50th iter or so
	
				ApplyPreconditioner( pJacobian, parameterVectorSize, pTemp->r.Elements( ), pTemp->MInvR.Elements( ) );
	
				Scalar errSqOld = pTemp->errSq;
				pTemp->errSq = pTemp->r.Dot( pTemp->MInvR );
				Scalar beta = pTemp->errSq / errSqOld;
				pTemp->d *= beta;
				pTemp->d += pTemp->MInvR;
	
				iteration++;
			}
	
			return iteration;
		}
	
	protected:
	
		void MultiplyByHessian(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY )
		{
			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
			for ( uint cameraStartIx = 0; cameraStartIx < cameraCount; )
			{
				Async::HessianCameraRowMultiplicationTask< CameraModel > task;
				task.Initialize( 512000 );
	
				task.InitializeWorkload( pJacobian, cameraStartIx, diagonalDampeningFactor, vectorSize, pX, pY );
	
				cameraStartIx += task.GetJacobian( ).GetCameraCount( );
	
				task.Execute( ); // TODO: execute task async
			}
	
			const uint pointCount = ( uint )pJacobian->GetPointCount( );
			for ( uint pointStartIx = 0; pointStartIx < pointCount; )
			{
				Async::HessianPointRowMultiplicationTask< CameraModel > task;
				task.Initialize( 512000 );
	
				task.InitializeWorkload( pJacobian, pointStartIx, diagonalDampeningFactor, vectorSize, pX, pY );
	
				pointStartIx += task.GetJacobian( ).GetPointCount( );
	
				task.Execute( ); // TODO: execute task async
			}
	
			// TODO: wait for all tasks to finish
		}
	
		void ApplyPreconditioner(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX )
		{
			HessianBlockProvider< CameraModel > hessian;
			hessian.Initialize( pJacobian );
	
			m_pPreconditioner->Apply( &hessian, vectorSize, pX, pPreconditionedX );
		}
	
		void GetInitialResidual(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_ecount( totalParamCount ) Scalar* pResidual )
		{
			MultiplyByHessian( pJacobian, diagonalDampeningFactor, parameterVectorSize, pX, pResidual );
			JtfNegSubtractX( pJacobian, parameterVectorSize, pResidual );
		}
	
	
		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		//                         ---------------------
		void JtfNegSubtractX(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint vectorSize,
			__inout_ecount( vectorSize ) Scalar* pX )
		{
			const size_t cameraCount = pJacobian->GetCameraCount( );
	
			Scalar camTemp[2 * CameraModel::cameraParameterCount];
			Scalar pointTemp[2 * POINT_PARAM_COUNT];
			Scalar residualTemp[2];
	
			Scalar camXTemp[CameraModel::cameraParameterCount];
			Scalar pointXTemp[POINT_PARAM_COUNT];
	
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				const size_t projectionCount = pJacobian->GetCameraProjectionCount( cameraIx );
				for ( size_t projI = 0; projI < projectionCount; projI++ )
				{
					const size_t projectionIx = pJacobian->GetCameraProjectionIndex( cameraIx, projI );
					const size_t pointIx = pJacobian->GetPointIndex( projectionIx );
	
					pJacobian->GetProjectionBlock< true, true, true >( projectionIx, camTemp, pointTemp, residualTemp );
	
					MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, 1 >( camTemp, residualTemp, camXTemp );
					MatrixMultiplyAtB< Scalar, 2, POINT_PARAM_COUNT, 1 >( pointTemp, residualTemp, pointXTemp );
	
					Scalar* pCamXDestination = Bundler::Utils::GetCameraParamPtr< CameraModel >( cameraIx, pX );
					Scalar* pPtXDestination = Bundler::Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pX );
	
					MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( camXTemp, pCamXDestination, pCamXDestination );
					MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pointXTemp, pPtXDestination, pPtXDestination );
				}
			}
	
			Vector< Scalar > x( vectorSize, pX );
			x.Negate( );
		}
	
	protected:
	
		uint m_maxIterations;
		Scalar m_errorToleranceSq;
	
		// const Preconditioner< CameraModel >* m_pPreconditioner;
	
		Async::WorkerPool* m_pWorkerPool;
	
	};

} } }
