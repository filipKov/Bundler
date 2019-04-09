#pragma once

namespace Bundler { namespace LinearSolver { namespace Internal {

	template < class CameraModel >
	class PCGLoopPart0CamerasTaskCPU : public Async::ITask
	{
	public:
	
		void Initialize(
			__in const uint cameraStartIx,
			__in const uint count,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = cameraStartIx;
			m_count = count;
	
			m_diagonalDampeningFactor = diagonalDampeningFactor;
	
			m_Ad = Utils::GetCameraParamPtr( cameraStartIx, pTemp->Ad.Elements( ) );
			m_d = pTemp->d.Elements( );
			m_pDotOut = &pTemp->dDotAd;
		}
	
		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
	
			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForCameras( m_pJacobian, m_startIx, m_count );
	
			LocalHessianMultiplicationEngineCPU< CameraModel > localHessian;
			localHessian.Initialize( &localJacobian, m_diagonalDampeningFactor );
	
			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const Scalar* pCameraD = m_d;
	
			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );
			const Scalar* pPointD = m_d + totalCameraParams;
	
			Scalar* pD = Utils::GetCameraParamPtr( m_startIx, m_d );
	
			Scalar partialDot = 0;
			for ( uint cameraIx = 0; cameraIx < localHessian.GetCameraCount( ); cameraIx++ )
			{
				localHessian.MultiplyCameraRow( cameraIx, totalCameraParams, pCameraD, totalPointParams, pPointD, m_Ad );
	
				partialDot += VectorDot< Scalar, cameraParamCount >( pD, m_Ad );
	
				pD += cameraParamCount;
				m_Ad += cameraParamCount;
			}
	
			m_pDotOut->operator+=( partialDot );
		}
	
	protected:
	
		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		uint m_count;
	
		Scalar m_diagonalDampeningFactor;
	
		Scalar* m_Ad;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pDotOut;
	};

	template < class CameraModel >
	class PCGLoopPart0PointsTaskCPU : public Async::ITask
	{
	public:

		void Initialize(
			__in const uint pointStartIx,
			__in const uint count,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = pointStartIx;
			m_count = count;

			m_diagonalDampeningFactor = diagonalDampeningFactor;

			m_Ad = Utils::GetPointParamPtr( pointStartIx, pJacobian->GetCameraCount(), pTemp->Ad.Elements( ) );
			m_d = pTemp->d.Elements( );
			m_pDotOut = &pTemp->dDotAd;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForPoints( m_pJacobian, m_startIx, m_count );

			LocalHessianMultiplicationEngineCPU< CameraModel > localHessian;
			localHessian.Initialize( &localJacobian, m_diagonalDampeningFactor );

			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const Scalar* pCameraD = m_d;

			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );
			const Scalar* pPointD = m_d + totalCameraParams;

			Scalar* pD = Utils::GetPointParamPtr( m_startIx, m_pJacobian->GetCameraCount(), m_d );

			Scalar partialDot = 0;
			for ( uint pointIx = 0; pointIx < localHessian.GetPointCount( ); pointIx++ )
			{
				localHessian.MultiplyPointRow( pointIx, totalCameraParams, pCameraD, totalPointParams, pPointD, m_Ad );

				partialDot += VectorDot< Scalar, POINT_PARAM_COUNT >( pD, m_Ad );

				pD += POINT_PARAM_COUNT;
				m_Ad += POINT_PARAM_COUNT;
			}

			m_pDotOut->operator+=( partialDot );
		}

	protected:

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		uint m_count;

		Scalar m_diagonalDampeningFactor;

		Scalar* m_Ad;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pDotOut;
	};

	template < class CameraModel >
	class PCGLoopPart1CamerasTaskCPU : public Async::ITask
	{
	public:

		void Initialize(
			__in const uint cameraStartIx,
			__in const uint count,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar alpha,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = cameraStartIx;
			m_count = count;

			m_alpha = alpha;
			m_x = Utils::GetCameraParamPtr( cameraStartIx, pX );

			m_r = Utils::GetCameraParamPtr( cameraStartIx, pTemp->r.Elements( ) );
			m_mr = Utils::GetCameraParamPtr( cameraStartIx, pTemp->MInvR.Elements( ) );
			m_pErrSqNew = &pTemp->errSqNew;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForCameras( m_pJacobian, m_startIx, m_count );

			LocalHessianBlockProviderCPU< CameraModel > localHessian;
			localHessian.Initialize( &localJacobian );

			Scalar errSqNewPart = 0;
			for ( uint cameraIx = 0; cameraIx < localJacobian->GetCameraCount(); cameraIx++ )
			{
				MatrixAddC< Scalar, cameraParamCount, 1 >( m_x, m_d, m_alpha, m_x );
				MatrixSubC< Scalar, cameraParamCount, 1 >( m_r, m_Ad, m_alpha, m_r );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToCamera( &localHessian, cameraIx, m_r, m_mr );

				errSqNewPart += VectorDot< Scalar, cameraParamCount >( m_r, m_mr );

				m_x += cameraParamCount;
				m_d += cameraParamCount;
				m_Ad += cameraParamCount;
				m_r += cameraParamCount;
				m_mr += cameraParamCount;
			}

			m_pErrSqNew->operator+=( errSqNewPart );
		}

	protected:

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		uint m_count;

		Scalar m_alpha;
		Scalar* m_x;

		Scalar* m_d;
		Scalar* m_Ad;
		Scalar* m_r;
		Scalar* m_mr;

		Async::InterlockedVariable< Scalar >* m_pErrSqNew;

	};

	template < class CameraModel >
	class PCGLoopPart1PointsTaskCPU : public Async::ITask
	{
	public:

		void Initialize(
			__in const uint pointStartIx,
			__in const uint count,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar alpha,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = pointStartIx;
			m_count = count;

			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );

			m_alpha = alpha;
			m_x = Utils::GetPointParamPtr( pointStartIx, cameraCount, pX );

			m_r = Utils::GetPointParamPtr( pointStartIx, cameraCount, pTemp->r.Elements( ) );
			m_mr = Utils::GetPointParamPtr( pointStartIx, cameraCount, pTemp->MInvR.Elements( ) );
			m_pErrSqNew = &pTemp->errSqNew;
		}

		void Execute( ) override
		{
			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForPoints( m_pJacobian, m_startIx, m_count );

			LocalHessianBlockProviderCPU< CameraModel > localHessian;
			localHessian.Initialize( &localJacobian );

			Scalar errSqNewPart = 0;
			for ( uint pointIx = 0; pointIx < localJacobian->GetPointCount( ); pointIx++ )
			{
				MatrixAddC< Scalar, POINT_PARAM_COUNT, 1 >( m_x, m_d, m_alpha, m_x );
				MatrixSubC< Scalar, POINT_PARAM_COUNT, 1 >( m_r, m_Ad, m_alpha, m_r );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToPoint( &localHessian, pointIx, m_r, m_mr );

				errSqNewPart += VectorDot< Scalar, POINT_PARAM_COUNT >( m_r, m_mr );

				m_x += POINT_PARAM_COUNT;
				m_d += POINT_PARAM_COUNT;
				m_Ad += POINT_PARAM_COUNT;
				m_r += POINT_PARAM_COUNT;
				m_mr += POINT_PARAM_COUNT;
			}

			m_pErrSqNew->operator+=( errSqNewPart );
		}

	protected:

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		uint m_count;

		Scalar m_alpha;
		Scalar* m_x;

		Scalar* m_d;
		Scalar* m_Ad;
		Scalar* m_r;
		Scalar* m_mr;

		Async::InterlockedVariable< Scalar >* m_pErrSqNew;

	};

} } }
