#pragma once

namespace Bundler { namespace LinearSolver { namespace Internal {

	template < class CameraModel >
	class PCGInitCamerasTaskCPU : public Async::ITask
	{
	public:
		
		void Initialize(
			__in const uint cameraStartIx,
			__in const uint count,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = cameraStartIx;
			m_count = count;

			m_diagonalDampeningFactor = diagonalDampeningFactor;

			UNREFERENCED_PARAMETER( parameterVectorSize );
			m_x0 = pX0;

			m_r = Utils::GetCameraParamPtr( cameraStartIx, pTemp->r.Elements( ) );
			m_d = Utils::GetCameraParamPtr( cameraStartIx, pTemp->d.Elements( ) );
			m_pErrSq = &pTemp->errSqNew;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
		
			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForCameras( m_pJacobian, m_startIx, m_count );

			LocalHessianMultiplicationEngineCPU< CameraModel > localHessian;
			localHessian.Initialize( &localJacobian, m_diagonalDampeningFactor );

			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const Scalar* pCameraX = m_x0;

			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );
			const Scalar* pPointX = m_x0 + totalCameraParams;

			Scalar errPart = 0;
			for ( uint cameraIx = 0; cameraIx < localJacobian.GetCameraCount( ); cameraIx++ )
			{
				localHessian.MultiplyCameraRow( cameraIx, totalCameraParams, pCameraX, totalPointParams, pPointX, m_r );

				JtfNegSubtractX( cameraIx, &localJacobian, m_r );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToCamera( &localHessian, cameraIx, m_r, m_d );

				errPart += VectorDot< Scalar, cameraParamCount >( m_r, m_d );
			}

			m_pErrSq->operator+=( errPart );
		}

	protected:

		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		//                         ---------------------
		void JtfNegSubtractX(
			__in const uint localCameraIx,
			__in const LocalProjectionProviderCPU< CameraModel >* pJacobian,
			__out_ecount( CameraModel::cameraPrameterCount ) Scalar* pDst )
		{
			Scalar camTemp[2 * CameraModel::cameraParameterCount];
			Scalar residualTemp[2];

			Scalar camXTemp[CameraModel::cameraParameterCount];

			const uint projectionCount = pJacobian->GetCameraProjectionCount( localCameraIx );
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = pJacobian->GetCameraProjectionIndex( localCameraIx, projI );
				
				pJacobian->GetProjectionBlock< true, false, true >( projectionIx, camTemp, NULL, residualTemp );

				MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, 1 >( camTemp, residualTemp, camXTemp );
				MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( camXTemp, pDst, pDst );
			}

			MatrixNegate< Scalar, CameraModel::cameraParameterCount, 1 >( pDst, pDst );
		}

	protected:

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		uint m_count;

		Scalar m_diagonalDampeningFactor;

		const Scalar* m_x0;

		Scalar* m_r;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pErrSq;
	};

	template < class CameraModel >
	class PCGInitPointsTaskCPU : public Async::ITask
	{
	public:

		void Initialize(
			__in const uint pointStartIx,
			__in const uint count,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__out ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = pointStartIx;
			m_count = count;

			m_diagonalDampeningFactor = diagonalDampeningFactor;

			UNREFERENCED_PARAMETER( parameterVectorSize );
			m_x0 = pX0;

			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
			m_r = Utils::GetPointParamPtr( pointStartIx, cameraCount, pTemp->r.Elements( ) );
			m_d = Utils::GetPointParamPtr( pointStartIx, cameraCount, pTemp->d.Elements( ) );
			m_pErrSq = &pTemp->errSqNew;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForPoints( m_pJacobian, m_startIx, m_count );

			LocalHessianMultiplicationEngineCPU< CameraModel > localHessian;
			localHessian.Initialize( &localJacobian, m_diagonalDampeningFactor );

			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const Scalar* pCameraX = m_x0;

			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );
			const Scalar* pPointX = m_x0 + totalCameraParams;

			Scalar errPart = 0;
			for ( uint pointIx = 0; pointIx < localJacobian.GetPointCount( ); pointIx++ )
			{
				localHessian.MultiplyPointRow( pointIx, totalCameraParams, pCameraX, totalPointParams, pPointX, m_r );

				JtfNegSubtractX( pointIx, &localJacobian, m_r );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToPoint( &localHessian, pointIx, m_r, m_d );

				errPart += VectorDot< Scalar, POINT_PARAM_COUNT >( m_r, m_d );
			}

			m_pErrSq->operator+=( errPart );
		}

	protected:

		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		//                         ---------------------
		void JtfNegSubtractX(
			__in const uint localPointIx,
			__in const LocalProjectionProviderCPU< CameraModel >* pJacobian,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pDst )
		{
			Scalar pointTemp[2 * POINT_PARAM_COUNT];
			Scalar residualTemp[2];

			Scalar pointXTemp[POINT_PARAM_COUNT];

			const uint projectionCount = pJacobian->GetPointProjectionCount( localPointIx );
			for ( uint projI = 0; projI < projectionCount; projI++ )
			{
				const uint projectionIx = pJacobian->GetPointProjectionIndex( localPointIx, projI );

				pJacobian->GetProjectionBlock< false, true, true >( projectionIx, pointTemp, NULL, residualTemp );

				MatrixMultiplyAtB< Scalar, 2, POINT_PARAM_COUNT, 1 >( pointTemp, residualTemp, pointXTemp );
				MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pointXTemp, pDst, pDst );
			}

			MatrixNegate< Scalar, POINT_PARAM_COUNT, 1 >( pDst, pDst );
		}

	protected:

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		uint m_count;

		Scalar m_diagonalDampeningFactor;

		const Scalar* m_x0;

		Scalar* m_r;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pErrSq;
	};

} } }