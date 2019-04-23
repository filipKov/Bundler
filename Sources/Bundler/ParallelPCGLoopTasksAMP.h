#pragma once

#pragma warning( push )
#pragma warning( disable: 4996 )

namespace Bundler { namespace LinearSolver { namespace Internal {

	template < class CameraModel >
	class PCGLoopPart0CamerasTaskAMP : public Async::ITask
	{
	public:

		PCGLoopPart0CamerasTaskAMP( __in accelerator_view dstAccelerator ) :
			m_accelerator( dstAccelerator )
		{}
	
		void Initialize(
			__in const uint cameraStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__inout ParallelPCGSolverTemp* pTemp ) __CPU_ONLY
		{
			m_pJacobian = pJacobian;
			m_startIx = cameraStartIx;
			m_counts = *pCounts;
	
			m_diagonalDampeningFactor = diagonalDampeningFactor;
	
			m_Ad = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->Ad.Elements( ) );
			m_d = pTemp->d.Elements( );
			m_pDotOut = &pTemp->dDotAd;
		}
	
		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
	
			LocalProjectionProviderDataAMP< CameraModel >* pJacobianData = NULL;
			LocalProjectionProviderDataAMP< CameraModel >::CreateForCameras( m_pJacobian, m_startIx, &m_counts, m_accelerator, &pJacobianData );

			LocalProjectionProviderAMP< CameraModel > localJacobian( pJacobianData );
	
			const uint cameraCount = localJacobian.GetCameraCount( );

			LocalHessianBlockProviderAMP< CameraModel > localHessianProvider( cameraCount, localJacobian.GetMaxProjectionCount( ), cameraParamCount );

			LocalHessianMultiplicationEngineAMP< CameraModel > localHessian;
			localHessian.Initialize( m_diagonalDampeningFactor );
	
			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );
			const uint startIx = m_startIx;

			array< Scalar, 1 > d( totalCameraParams + totalPointParams, m_d, m_accelerator );
			array< Scalar, 1 > Ad( cameraCount * cameraParamCount, m_Ad, m_accelerator );
			array< Scalar, 1 > partialDots( cameraCount, m_accelerator );

			parallel_for_each( m_accelerator, extent< 1 >( cameraCount ),
				[ localJacobian, localHessianProvider, localHessian, totalCameraParams, totalPointParams, startIx, &d, &Ad, &partialDots ]
			( index< 1 > localCameraIx ) restrict( amp )
			{
				const uint cameraParamCountKernel = CameraModel::cameraParameterCount;

				const Scalar* pCameraD = d.data( );
				const Scalar* pPointD = pCameraD + totalCameraParams;
				const Scalar* pD = Utils::GetCameraParamPtr< CameraModel >( startIx + localCameraIx[0], pCameraD );

				Scalar* pAd = Ad.data( ) + localCameraIx[0] * cameraParamCountKernel;

				localHessian.MultiplyCameraRow( &localJacobian, &localHessianProvider, localCameraIx[0], totalCameraParams, pCameraD, totalPointParams, pPointD, pAd );
				
				partialDots[localCameraIx] = VectorDot< Scalar, cameraParamCountKernel >( pD, pAd );
			} );

			m_accelerator.wait( );
			
			copy( Ad, m_Ad );
	
			Scalar partialDot = Async::AccumulateBuffer( cameraCount, partialDots );
			m_pDotOut->operator+=( partialDot );

			delete pJacobianData;
		}
	
	protected:
	
		accelerator_view m_accelerator;

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		LocalProjectionProviderDataCountsAMP m_counts;
	
		Scalar m_diagonalDampeningFactor;
	
		Scalar* m_Ad;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pDotOut;
	};

	template < class CameraModel >
	class PCGLoopPart0PointsTaskAMP : public Async::ITask
	{
	public:

		PCGLoopPart0PointsTaskAMP( __in accelerator_view dstAccelerator ) :
			m_accelerator( dstAccelerator )
		{}

		void Initialize(
			__in const uint pointStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = pointStartIx;
			m_counts = *pCounts;

			m_diagonalDampeningFactor = diagonalDampeningFactor;

			m_Ad = Utils::GetPointParamPtr< CameraModel >( pointStartIx, pJacobian->GetCameraCount(), pTemp->Ad.Elements( ) );
			m_d = pTemp->d.Elements( );
			m_pDotOut = &pTemp->dDotAd;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderDataAMP< CameraModel >* pJacobianData = NULL;
			LocalProjectionProviderDataAMP< CameraModel >::CreateForPoints( m_pJacobian, m_startIx, &m_counts, m_accelerator, &pJacobianData );

			LocalProjectionProviderAMP< CameraModel > localJacobian( pJacobianData );

			const uint pointCount = localJacobian.GetPointCount( );

			LocalHessianBlockProviderAMP< CameraModel > localHessianProvider( pointCount, localJacobian.GetMaxProjectionCount( ), POINT_PARAM_COUNT );

			LocalHessianMultiplicationEngineAMP< CameraModel > localHessian;
			localHessian.Initialize( m_diagonalDampeningFactor );

			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );
			const uint startIx = m_startIx;

			array< Scalar, 1 > d( totalCameraParams + totalPointParams, m_d, m_accelerator );
			array< Scalar, 1 > Ad( pointCount * POINT_PARAM_COUNT, m_Ad, m_accelerator );
			array< Scalar, 1 > partialDots( pointCount, m_accelerator );

			parallel_for_each( m_accelerator, extent< 1 >( pointCount ),
				[ localJacobian, localHessianProvider, localHessian, totalCameraParams, totalPointParams, startIx, &d, &Ad, &partialDots ]
			( index< 1 > localPointIx ) restrict( amp )
			{
				constexpr uint pointParamCount = POINT_PARAM_COUNT;

				const Scalar* pCameraD = d.data( );
				const Scalar* pPointD = pCameraD + totalCameraParams;
				const Scalar* pD = Utils::GetPointParamPtr< CameraModel >( startIx + localPointIx[0], localJacobian.GetCameraCount( ), pCameraD );

				Scalar* pAd = Ad.data( ) + localPointIx[0] * pointParamCount;

				localHessian.MultiplyPointRow( &localJacobian, &localHessianProvider, localPointIx[0], totalCameraParams, pCameraD, totalPointParams, pPointD, pAd );

				partialDots[localPointIx] = VectorDot< Scalar, pointParamCount >( pD, pAd );
			} );

			m_accelerator.wait( );

			copy( Ad, m_Ad );
			
			Scalar partialDot = Async::AccumulateBuffer( pointCount, partialDots );
			m_pDotOut->operator+=( partialDot );

			delete pJacobianData;
		}

	protected:

		accelerator_view m_accelerator;

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		LocalProjectionProviderDataCountsAMP m_counts;

		Scalar m_diagonalDampeningFactor;

		Scalar* m_Ad;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pDotOut;
	};

	template < class CameraModel >
	class PCGLoopPart1CamerasTaskAMP : public Async::ITask
	{
	public:

		PCGLoopPart1CamerasTaskAMP( __in accelerator_view dstAccelerator ) :
			m_accelerator( dstAccelerator )
		{}

		void Initialize(
			__in const uint cameraStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar alpha,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			
			__inout ParallelPCGSolverTemp* pTemp )
		{
			UNREFERENCED_PARAMETER( parameterVectorSize );

			m_pJacobian = pJacobian;
			m_startIx = cameraStartIx;
			m_counts = *pCounts;

			m_alpha = alpha;
			m_x = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pX );

			m_d = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->d.Elements( ) );
			m_Ad = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->Ad.Elements( ) );
			m_r = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->r.Elements( ) );
			m_mr = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->MInvR.Elements( ) );
			m_pErrSqNew = &pTemp->errSqNew;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderDataAMP< CameraModel >* pJacobianData = NULL;
			LocalProjectionProviderDataAMP< CameraModel >::CreateForCameras( m_pJacobian, m_startIx, &m_counts, m_accelerator, &pJacobianData );

			LocalProjectionProviderAMP< CameraModel > localJacobian( pJacobianData );

			const uint cameraCount = localJacobian.GetCameraCount( );

			LocalHessianBlockProviderAMP< CameraModel > localHessian( cameraCount, localJacobian.GetMaxProjectionCount( ), cameraParamCount );

			const uint blockParamCount = cameraCount * cameraParamCount;
			const Scalar alpha = m_alpha;

			array< Scalar, 1 > x( blockParamCount, m_x, m_accelerator );
			array< Scalar, 1 > d( blockParamCount, m_d, m_accelerator );
			array< Scalar, 1 > r( blockParamCount, m_r, m_accelerator );
			array< Scalar, 1 > Ad( blockParamCount, m_Ad, m_accelerator );
			array< Scalar, 1 > mr( blockParamCount, m_mr, m_accelerator );
			array< Scalar, 1 > newErrSqParts( cameraCount, m_accelerator );

			parallel_for_each( m_accelerator, extent< 1 >( cameraCount ),
				[ localJacobian, localHessian, alpha, &x, &d, &r, &Ad, &mr, &newErrSqParts ]
				( index< 1 > ix ) restrict( amp )
			{
				constexpr uint cameraParamCountKernel = CameraModel::cameraParameterCount;

				const uint localCameraIx = ix[0];

				const Scalar* pD = d.data( ) + localCameraIx * cameraParamCountKernel;
				const Scalar* pAd = Ad.data( ) + localCameraIx * cameraParamCountKernel;

				Scalar* pX = x.data( ) + localCameraIx * cameraParamCountKernel;
				Scalar* pR = r.data( ) + localCameraIx * cameraParamCountKernel;
				Scalar* pMr = mr.data( ) + localCameraIx * cameraParamCountKernel;

				MatrixAddC< Scalar, cameraParamCountKernel, 1 >( pX, pD, alpha, pX );
				MatrixSubC< Scalar, cameraParamCountKernel, 1 >( pR, pAd, alpha, pR );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToCamera( &localJacobian, &localHessian, localCameraIx, pR, pMr );

				newErrSqParts[ ix ] = VectorDot< Scalar, cameraParamCountKernel >( pR, pMr );
			} );

			m_accelerator.wait( );

			copy( x, m_x );
			copy( r, m_r );
			copy( mr, m_mr );

			const Scalar errSqNewPart = Async::AccumulateBuffer( cameraCount, newErrSqParts );
			m_pErrSqNew->operator+=( errSqNewPart );

			delete pJacobianData;
		}

	protected:

		accelerator_view m_accelerator;

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		LocalProjectionProviderDataCountsAMP m_counts;

		Scalar m_alpha;
		Scalar* m_x;

		Scalar* m_d;
		Scalar* m_Ad;
		Scalar* m_r;
		Scalar* m_mr;

		Async::InterlockedVariable< Scalar >* m_pErrSqNew;

	};

	template < class CameraModel >
	class PCGLoopPart1PointsTaskAMP : public Async::ITask
	{
	public:

		PCGLoopPart1PointsTaskAMP( __in accelerator_view dstAccelerator ) :
			m_accelerator( dstAccelerator )
		{}

		void Initialize(
			__in const uint pointStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar alpha,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__inout ParallelPCGSolverTemp* pTemp )
		{
			UNREFERENCED_PARAMETER( parameterVectorSize );

			m_pJacobian = pJacobian;
			m_startIx = pointStartIx;
			m_counts = *pCounts;

			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );

			m_alpha = alpha;
			m_x = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pX );

			m_d = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pTemp->d.Elements( ) );
			m_Ad = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pTemp->Ad.Elements( ) );
			m_r = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pTemp->r.Elements( ) );
			m_mr = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pTemp->MInvR.Elements( ) );
			m_pErrSqNew = &pTemp->errSqNew;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderDataAMP< CameraModel >* pJacobianData = NULL;
			LocalProjectionProviderDataAMP< CameraModel >::CreateForPoints( m_pJacobian, m_startIx, &m_counts, m_accelerator, &pJacobianData );

			LocalProjectionProviderAMP< CameraModel > localJacobian( pJacobianData );

			const uint pointCount = localJacobian.GetPointCount( );

			LocalHessianBlockProviderAMP< CameraModel > localHessian( pointCount, localJacobian.GetMaxProjectionCount( ), POINT_PARAM_COUNT );

			const uint blockParamCount = pointCount * POINT_PARAM_COUNT;
			const Scalar alpha = m_alpha;

			array< Scalar, 1 > x( blockParamCount, m_x, m_accelerator );
			array< Scalar, 1 > d( blockParamCount, m_d, m_accelerator );
			array< Scalar, 1 > r( blockParamCount, m_r, m_accelerator );
			array< Scalar, 1 > Ad( blockParamCount, m_Ad, m_accelerator );
			array< Scalar, 1 > mr( blockParamCount, m_mr, m_accelerator );
			array< Scalar, 1 > newErrSqParts( pointCount, m_accelerator );

			parallel_for_each( m_accelerator, extent< 1 >( pointCount ),
				[ localJacobian, localHessian, alpha, &x, &d, &r, &Ad, &mr, &newErrSqParts ]
			( index< 1 > ix ) restrict( amp )
			{
				constexpr uint pointParamCount = POINT_PARAM_COUNT;

				const uint localPointIx = ix[0];

				const Scalar* pD = d.data( ) + localPointIx * pointParamCount;
				const Scalar* pAd = Ad.data( ) + localPointIx * pointParamCount;

				Scalar* pX = x.data( ) + localPointIx * pointParamCount;
				Scalar* pR = r.data( ) + localPointIx * pointParamCount;
				Scalar* pMr = mr.data( ) + localPointIx * pointParamCount;

				MatrixAddC< Scalar, pointParamCount, 1 >( pX, pD, alpha, pX );
				MatrixSubC< Scalar, pointParamCount, 1 >( pR, pAd, alpha, pR );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToPoint( &localJacobian, &localHessian, localPointIx, pR, pMr );

				newErrSqParts[ix] = VectorDot< Scalar, pointParamCount >( pR, pMr );
			} );

			m_accelerator.wait( );

			copy( x, m_x );
			copy( r, m_r );
			copy( mr, m_mr );

			const Scalar errSqNewPart = Async::AccumulateBuffer( pointCount, newErrSqParts );
			m_pErrSqNew->operator+=( errSqNewPart );

			delete pJacobianData;
		}

	protected:

		accelerator_view m_accelerator;

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		LocalProjectionProviderDataCountsAMP m_counts;

		Scalar m_alpha;
		Scalar* m_x;

		Scalar* m_d;
		Scalar* m_Ad;
		Scalar* m_r;
		Scalar* m_mr;

		Async::InterlockedVariable< Scalar >* m_pErrSqNew;

	};

} } }

#pragma warning( pop )
