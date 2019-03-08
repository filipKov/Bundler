#pragma once

namespace Bundler { namespace LinearSolver {

	template < class CameraModel >
	class PCGSolver 
	{
	public:
		
		void Initialize(
			__in const uint maxIterations,
			__in const Scalar errorTolerance ) 
		{
			m_maxIterations = maxIterations;
			m_errorToleranceSq = errorTolerance * errorTolerance;
		}

		void SolveSystem(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_opt PCGSolverStatistics* pStatistics ) const
		{
			PCGSolverTemp temp( parameterVectorSize );

			HessianMultiplicationEngine< CameraModel > hessian;
			hessian.Initialize( pJacobian );
			
			InitSolve( pJacobian, &hessian, parameterVectorSize, pX, &temp );
			uint finalIter = SolveLoop( pJacobian, &hessian, parameterVectorSize, pX, &tmep );

			if ( pStatistics )
			{
				pStatistics->iterationCount = finalIter;
				pStatistics->finalError = sqrt( temp.errSq );
			}
		}

		void InitSolve(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out PCGSolverTemp* pTemp )
		{
			GetInitialResidual( pJacobian, pHessian, parameterVectorSize, pX, pTemp->r.Elements() );
			ApplyPreconditioner( pHessian, pTemp->r, pTemp->d );
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
					( pTemp->m_errSq > m_errorToleranceSq ) )
			{
				MultiplyByHessian( pHessian, pTemp->d.Elements(), pTemp->Ad.Elements() );

				Scalar alpha = pTemp->errSq / ( pTemp->d.Dot( pTemp->Ad ) );
				x += alpha * pTemp->d;
				pTemp->r -= alpha * pTemp->Ad; // maybe do "r = b - Ax" every 50th iter or so

				ApplyPreconditioner( pHessian, parameterVectorSize, pTemp->r.Elements(), pTemp->MInvR.Elements() );

				Scalar errSqOld = pTemp->errSq;
				pTemp->errSq = pTemp->r.Dot( pTemp->MInvR );
				Scalar beta = temp.errSq / errSqOld;
				pTemp->d *= beta;
				pTemp->d += pTemp->MInvR;

				iteration++;
			}

			return iteration;
		}

	protected:

		void MultiplyByHessian(
			__in HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY )
		{
			Scalar pYDestination = pY;

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
			__in HessianMultiplicationEngine< CameraModel >* pHessian,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pPreconditionedX )
		{
			// TODO: Jacobi preconditioner !!!
			ShallowCopy< Scalar >( pX, vectorSize, pPreconditionedX ); // TODO: delete this;
		}

		void GetInitialResidual(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const IHessianProvider< CameraModel >* pHessian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_ecount( totalParamCount ) Scalar* pResidual )
		{
			MultiplyByHessian( pHessian, parameterVectorSize, pX, pResidual );
			JtfNegSubtractX( pJacobian, parameterVectorSize, pResidual );
		}
		

		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		void JtfNegSubtractX(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint vectorSize,
			__inout_ecount( vectorSize ) Scalar* pX )
		{
			const size_t cameraCount = pJacobian->GetCameraCount();

			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				const size_t projectionCount = pJacobian->GetCameraProjectionCount( cameraIx );
				for ( size_t projI = 0; projI < projectionCount; projI++ )
				{
					const size_t projectionIx = pJacobian->GetCameraProjectionIndex( cameraIx, projI );

					// TODO: !!
					pJacobian->GetProjectionBlock< true, true, true >( projectionIx, ..., ..., ... );
				}
			}

			Vector< Scalar > x( vectorSize, pX );
			x.Negate();
		}

	protected:
		
		uint m_maxIterations;
		Scalar m_errorToleranceSq;

	};

} }