#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class PCGSolver 
	{
	public:
		
		void Initialize(
			__in const uint maxIterations,
			__in const Scalar errorTolerance,
			__in const Preconditioner< CameraModel >* pPreconditioner ) 
		{
			m_maxIterations = maxIterations;
			m_errorToleranceSq = errorTolerance * errorTolerance;
			m_pPreconditioner = pPreconditioner;
		}

		void Initialize(
			__in const PCGSolverSettings& settings,
			__in const Preconditioner< CameraModel >* pPreconditioner )
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
			GetInitialResidual( pJacobian, pHessian, parameterVectorSize, pX0, pTemp->r.Elements() );
			ApplyPreconditioner( pJacobian, parameterVectorSize, pTemp->r.Elements(), pTemp->d.Elements() );
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

			uint iteration = 0;
			while ( ( iteration < m_maxIterations ) && 
					( pTemp->errSq > m_errorToleranceSq ) )
			{
				MultiplyByHessian( pHessian, parameterVectorSize, pTemp->d.Elements(), pTemp->Ad.Elements() );

				Scalar alpha = pTemp->errSq / ( pTemp->d.Dot( pTemp->Ad ) );
				x += alpha * pTemp->d;
				pTemp->r -= alpha * pTemp->Ad; // maybe do "r = b - Ax" every 50th iter or so

				ApplyPreconditioner( pJacobian, parameterVectorSize, pTemp->r.Elements(), pTemp->MInvR.Elements() );

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
			__in const HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY ) const
		{
			Scalar* pYDestination = pY;

			const size_t cameraCount = pHessian->GetCameraCount();
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				pHessian->MultiplyCameraRow( cameraIx, vectorSize, pX, pYDestination );
				pYDestination += CameraModel::cameraParameterCount;
			}

			const size_t pointCount = pHessian->GetPointCount();
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				pHessian->MultiplyPointRow( pointIx, vectorSize, pX, pYDestination );
				pYDestination += POINT_PARAM_COUNT;
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

			m_pPreconditioner->Apply( &hessian, vectorSize, pX, pPreconditionedX );
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
			const size_t cameraCount = pJacobian->GetCameraCount();

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
			x.Negate();
		}

	protected:
		
		uint m_maxIterations;
		Scalar m_errorToleranceSq;

		const Preconditioner< CameraModel >* m_pPreconditioner;

	};

} }