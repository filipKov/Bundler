#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class PCGSolverOpenMP
	{
	public:
	
		void Initialize(
			__in const uint maxIterations,
			__in const Scalar errorTolerance )
		{
			m_maxIterations = maxIterations;
			m_errorTolerance = errorTolerance;
			m_pPreconditioner = pPreconditioner;
		}
	
		void Initialize( __in const PCGSolverSettings& settings )
		{
			Initialize( settings.maxIterations, settings.errorTolerance, pPreconditioner );
		}
	
		void SolveSystem(
			__in const Scalar diagonalDampeningFactor,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_opt PCGSolverStatistics* pStatistics ) const
		{
			PCGSolverTemp temp( parameterVectorSize );
	
			HessianMultiplicationEngine< CameraModel > hessian;
			hessian.Initialize( pJacobian, diagonalDampeningFactor );
	
			InitSolve( pJacobian, &hessian, parameterVectorSize, pX, &temp );
			uint finalIter = SolveLoop( pJacobian, &hessian, parameterVectorSize, pX, &temp );
	
			if ( pStatistics )
			{
				pStatistics->iterationCount = finalIter;
				pStatistics->finalError = sqrt( temp.errSq );
			}
		}
	
	protected:
	
		void InitSolve(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out PCGSolverTemp* pTemp ) const
		{
			GetInitialResidual( pJacobian, pHessian, parameterVectorSize, pX0, pTemp->r.Elements( ) );
			ApplyPreconditioner( pJacobian, parameterVectorSize, pTemp->r.Elements( ), pTemp->d.Elements( ) );
			pTemp->errSq = pTemp->r.Dot( pTemp->d );
		}
	
		uint SolveLoop(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout PCGSolverTemp* pTemp ) const
		{
			Vector< Scalar > x( parameterVectorSize, pX );
	
			HighResolutionClock stopwatch;
	
			Scalar minError = m_errorTolerance * pTemp->errSq;
	
			uint iteration = 0;
			while ( ( iteration < m_maxIterations ) &&
				( pTemp->errSq > minError ) )
			{
				stopwatch.Start( );
	
				MultiplyByHessian( pHessian, parameterVectorSize, pTemp->d.Elements( ), pTemp->Ad.Elements( ) );
	
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
	
				stopwatch.Stop( );
				printf_s( "PCG loop time: %fms\n", stopwatch.GetTotalTime( ) );
				stopwatch.Clear( );
			}
	
			return iteration;
		}
	
	protected:
	
		void MultiplyByHessian(
			__in const HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY ) const
		{
			const int64 cameraCount = pHessian->GetCameraCount( );
			const int64 pointCount = pHessian->GetPointCount( );
	
			#pragma omp parallel
			{
	
				#pragma omp for nowait
				for ( int64 cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
				{
					Scalar* pYDestination = Utils::GetCameraParamPtr< CameraModel >( cameraIx, pY );
					pHessian->MultiplyCameraRow( cameraIx, vectorSize, pX, pYDestination );
				}
	
				#pragma omp for
				for ( int64 pointIx = 0; pointIx < pointCount; pointIx++ )
				{
					Scalar* pYDestination = Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pY );
					pHessian->MultiplyPointRow( pointIx, vectorSize, pX, pYDestination );
				}
			}
		}
	
		void ApplyPreconditioner(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX ) const
		{
			HessianBlockProvider< CameraModel > hessian;
			hessian.Initialize( pJacobian );
	
			BlockJacobiPreconditionerOpenMP< CameraModel >::Apply( &hessian, vectorSize, pX, pPreconditionedX );
		}
	
		void GetInitialResidual(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out_ecount( totalParamCount ) Scalar* pResidual ) const
		{
			MultiplyByHessian( pHessian, parameterVectorSize, pX0, pResidual );
			JtfNegSubtractX( pJacobian, parameterVectorSize, pResidual );
		}
	
	
		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		void JtfNegSubtractX(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint vectorSize,
			__inout_ecount( vectorSize ) Scalar* pX ) const
		{
			const int64 cameraCount = (int64)pJacobian->GetCameraCount( );
	
			#pragma omp parallel
			{
				Scalar camTemp[2 * CameraModel::cameraParameterCount];
				Scalar pointTemp[2 * POINT_PARAM_COUNT];
				Scalar residualTemp[2];

				Scalar camXTemp[CameraModel::cameraParameterCount];
				Scalar pointXTemp[POINT_PARAM_COUNT];

				#pragma omp for
				for ( int64 cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
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
						
						#pragma omp critical
						{
							MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pointXTemp, pPtXDestination, pPtXDestination );
						}
					}
				}
			}
	
			Vector< Scalar > x( vectorSize, pX );
			x.Negate( );
		}
	
	protected:
	
		uint m_maxIterations;
		Scalar m_errorTolerance;
	
	};

} }
