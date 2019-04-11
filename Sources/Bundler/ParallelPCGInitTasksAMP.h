#pragma once

namespace Bundler { namespace LinearSolver { namespace Internal {

	template < class CameraModel >
	class PCGInitCamerasTaskAMP : public Async::ITask
	{
	public:
		
		void Initialize(
			__in const uint cameraStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__in accelerator_view* pDstAccelerator,
			__out ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = cameraStartIx;
			m_counts = *pCounts;

			m_diagonalDampeningFactor = diagonalDampeningFactor;

			UNREFERENCED_PARAMETER( parameterVectorSize );
			m_x0 = pX0;

			m_r = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->r.Elements( ) );
			m_d = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pTemp->d.Elements( ) );
			m_pErrSq = &pTemp->errSqNew;

			m_pAccelerator = pDstAccelerator;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderDataAMP< CameraModel >* pJacobianData;
			LocalProjectionProviderDataAMP< CameraModel >::CreateForCameras( m_pJacobian, m_startIx, &m_counts, *m_pAccelerator, &pJacobianData );

			LocalProjectionProviderAMP< CameraModel > localJacobian( pJacobianData );

			const uint cameraCount = localJacobian.GetCameraCount( );

			LocalHessianBlockProviderAMP< CameraModel > localHessianProvider( cameraCount, localJacobian.GetMaxProjectionCount( ), cameraParamCount );

			LocalHessianMultiplicationEngineAMP< CameraModel > localHessian;
			localHessian.Initialize( m_diagonalDampeningFactor );

			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );

			array< Scalar, 1 > r( cameraCount * cameraParamCount, m_r, *m_pAccelerator );
			array< Scalar, 1 > d( cameraCount * cameraParamCount, m_d, *m_pAccelerator );
			array< Scalar, 1 > x( totalCameraParams + totalPointParams, m_x0, *m_pAccelerator );
			array< Scalar, 1 > errParts( cameraCount, *m_pAccelerator );

			parallel_for_each( *m_pAccelerator, extent< 1 >( cameraCount ),
				[ localJacobian, localHessianProvider, localHessian, totalCameraParams, totalPointParams, &r, &x, &errParts ]
			( index< 1 > ix ) restrict( amp )
			{
				constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

				const uint localCameraIx = ix[0];

				const Scalar* pCameraX = x.data( );
				const Scalar* pPointX = pCameraX + totalCameraParams;

				Scalar* pR = r.data( ) + localCameraIx * cameraParamCount;
				
				localHessian.MultiplyCameraRow( &localJacobian, &localHessianProvider, localCameraIx, totalCameraParams, pCameraX, totalPointParams, pPointX, pR );
			} );

			parallel_for_each( *m_pAccelerator, extent< 1 >( cameraCount ),
				[ localJacobian, localHessianProvider, &r, &d, &errParts ]
			( index< 1 > ix ) restrict( amp )
			{
				constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

				const uint localCameraIx = ix[0];

				Scalar* pR = r.data( ) + localCameraIx * cameraParamCount;
				Scalar* pD = d.data( ) + localCameraIx * cameraParamCount;

				JtfNegSubtractX( localCameraIx, &localJacobian, pR );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToCamera( &localJacobian, &localHessianProvider, localCameraIx, pR, pD );

				errParts[ix] = VectorDot< Scalar, cameraParamCount >( pR, pD );
			} );

			m_pAccelerator->wait( );

			copy( r, m_r );
			copy( d, m_d );

			const Scalar errPart = Async::AccumulateBuffer( cameraCount, errParts );
			m_pErrSq->operator+=( errPart );
		}

	protected:

		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		//                         ---------------------
		static void JtfNegSubtractX(
			__in const uint localCameraIx,
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__out_ecount( CameraModel::cameraPrameterCount ) Scalar* pDst ) restrict( amp )
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

		accelerator_view* m_pAccelerator;

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		LocalProjectionProviderDataCountsAMP m_counts;

		Scalar m_diagonalDampeningFactor;

		const Scalar* m_x0;

		Scalar* m_r;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pErrSq;
	};

	template < class CameraModel >
	class PCGInitPointsTaskAMP : public Async::ITask
	{
	public:

		void Initialize(
			__in const uint pointStartIx,
			__in const LocalProjectionProviderDataCountsAMP* pCounts,
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const Scalar diagonalDampeningFactor,
			__in const uint parameterVectorSize,
			__in_ecount( parameterVectorSize ) const Scalar* pX0,
			__in accelerator_view* pDstAccelerator,
			__out ParallelPCGSolverTemp* pTemp )
		{
			m_pJacobian = pJacobian;
			m_startIx = pointStartIx;
			m_counts = *pCounts

			m_diagonalDampeningFactor = diagonalDampeningFactor;

			UNREFERENCED_PARAMETER( parameterVectorSize );
			m_x0 = pX0;

			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
			m_r = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pTemp->r.Elements( ) );
			m_d = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pTemp->d.Elements( ) );
			m_pErrSq = &pTemp->errSqNew;

			m_pAccelerator = pDstAccelerator;
		}

		void Execute( ) override
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

			LocalProjectionProviderDataAMP< CameraModel >* pJacobianData;
			LocalProjectionProviderDataAMP< CameraModel >::CreateForPoints( m_pJacobian, m_startIx, &m_counts, *m_pAccelerator, &pJacobianData );

			LocalProjectionProviderAMP< CameraModel > localJacobian( pJacobianData );

			const uint pointCount = localJacobian.GetPointCount( );

			LocalHessianBlockProviderAMP< CameraModel > localHessianProvider( pointCount, localJacobian.GetMaxProjectionCount( ), POINT_PARAM_COUNT );

			LocalHessianMultiplicationEngineAMP< CameraModel > localHessian;
			localHessian.Initialize( m_diagonalDampeningFactor );

			const uint totalCameraParams = cameraParamCount * ( uint )m_pJacobian->GetCameraCount( );
			const uint totalPointParams = POINT_PARAM_COUNT * ( uint )m_pJacobian->GetPointCount( );

			array< Scalar, 1 > r( pointCount * POINT_PARAM_COUNT, m_r, *m_pAccelerator );
			array< Scalar, 1 > d( pointCount * POINT_PARAM_COUNT, m_d, *m_pAccelerator );
			array< Scalar, 1 > x( totalCameraParams + totalPointParams, m_x0, *m_pAccelerator );
			array< Scalar, 1 > errParts( pointCount, *m_pAccelerator );


			parallel_for_each( *m_pAccelerator, extent< 1 >( pointCount ),
				[ localJacobian, localHessianProvider, localHessian, totalCameraParams, totalPointParams, &r, &x, &errParts ]
			( index< 1 > ix ) restrict( amp )
			{
				constexpr const uint pointParamCount = POINT_PARAM_COUNT;

				const uint localPointIx = ix[0];

				const Scalar* pCameraX = x.data( );
				const Scalar* pPointX = pCameraX + totalCameraParams;

				Scalar* pR = r.data( ) + localPointIx * pointParamCount;

				localHessian.MultiplyCameraRow( &localJacobian, &localHessianProvider, localPointIx, totalCameraParams, pCameraX, totalPointParams, pPointX, pR );
			} );

			parallel_for_each( *m_pAccelerator, extent< 1 >( pointCount ),
				[ localJacobian, localHessianProvider, localHessian, &r, &d, &errParts ]
			( index< 1 > ix ) restrict( amp )
			{
				constexpr const uint pointParamCount = POINT_PARAM_COUNT;

				const uint localPointIx = ix[0];

				Scalar* pR = r.data( ) + localPointIx * pointParamCount;
				Scalar* pD = d.data( ) + localPointIx * pointParamCount;

				JtfNegSubtractX( localPointIx, &localJacobian, pR );

				LocalBlockJacobiPreconditioner< CameraModel >::ApplyToPoint( &localJacobian, &localHessianProvider, localPointIx, pR, pD );

				errParts[ix] = VectorDot< Scalar, pointParamCount >( pR, pD );
			} );

			m_pAccelerator->wait( );

			copy( r, m_r );
			copy( d, m_d );

			const Scalar errPart = Async::AccumulateBuffer( pointCount, errParts );
			m_pErrSq->operator+=( errPart );
		}

	protected:

		// ( x = -J^Tf - x )  <=>  ( x = -( x + J^Tf ) )
		//                         ---------------------
		static void JtfNegSubtractX(
			__in const uint localPointIx,
			__in const LocalProjectionProviderAMP< CameraModel >* pJacobian,
			__out_ecount( POINT_PARAM_COUNT ) Scalar* pDst ) restrict( amp )
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

		accelerator_view* m_pAccelerator;

		const ProjectionProvider< CameraModel >* m_pJacobian;
		uint m_startIx;
		LocalProjectionProviderDataCountsAMP m_counts;

		Scalar m_diagonalDampeningFactor;

		const Scalar* m_x0;

		Scalar* m_r;
		Scalar* m_d;
		Async::InterlockedVariable< Scalar >* m_pErrSq;
	};

} } }
