#pragma once

class HxCamerasGPUTask : public IEvaluatorTask
{
public:

	HxCamerasGPUTask( __in accelerator_view dstAccelerator ) :
		m_accelerator( dstAccelerator )
	{}

	~HxCamerasGPUTask( )
	{
		delete m_pVectorY;
		delete m_pJacobianData;
	}

	void PreInit( __in const TaskInput* pIn ) override
	{
		m_pVectorY = new Vector< Scalar >( pIn->vectorSize );

		m_cameraCount = (uint)min( pIn->maxTaskSize, ( int64 )pIn->pJacobian->GetCameraCount( ) );
		LocalProjectionProviderDataAMP< CameraModel >::GetCountsForCameraBatch( pIn->pJacobian, 0, m_cameraCount, &m_counts );
	}

	int64 Initialize( __in const TaskInput* pIn ) override
	{
		LocalProjectionProviderDataAMP< CameraModel >::CreateForCameras( pIn->pJacobian, 0, &m_counts, m_accelerator, &m_pJacobianData );
		
		m_Ad = m_pVectorY->Elements( );

		return m_cameraCount;
	}

	void Execute( __in const TaskInput* pIn ) override
	{
		constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

		LocalProjectionProviderAMP< CameraModel > localJacobian( m_pJacobianData );
		LocalHessianBlockProviderAMP< CameraModel > localHessianProvider( m_cameraCount, localJacobian.GetMaxProjectionCount( ), cameraParamCount );

		LocalHessianMultiplicationEngineAMP< CameraModel > localHessian;
		localHessian.Initialize( pIn->dampening );

		const uint totalCameraParams = cameraParamCount * ( uint )pIn->pJacobian->GetCameraCount( );
		const uint totalPointParams = POINT_PARAM_COUNT * ( uint )pIn->pJacobian->GetPointCount( );
		const uint startIx = 0;

		array< Scalar, 1 > d( totalCameraParams + totalPointParams, pIn->pVectorX, m_accelerator );
		array< Scalar, 1 > Ad( m_cameraCount * cameraParamCount, m_accelerator );
		
		parallel_for_each( m_accelerator, extent< 1 >( m_cameraCount ),
			[ localJacobian, localHessianProvider, localHessian, totalCameraParams, totalPointParams, startIx, &d, &Ad ]
		( index< 1 > localCameraIx ) restrict( amp )
		{
			const uint cameraParamCountKernel = CameraModel::cameraParameterCount;

			const Scalar* pCameraD = d.data( );
			const Scalar* pPointD = pCameraD + totalCameraParams;

			Scalar* pAd = Ad.data( ) + localCameraIx[0] * cameraParamCountKernel;

			localHessian.MultiplyCameraRow( &localJacobian, &localHessianProvider, localCameraIx[0], totalCameraParams, pCameraD, totalPointParams, pPointD, pAd );
		} );

		m_accelerator.wait( );

		copy( Ad, m_Ad );
	}

protected:

	accelerator_view m_accelerator;

	Vector< Scalar >* m_pVectorY;

	uint m_cameraCount; 
	
	LocalProjectionProviderDataCountsAMP m_counts;
	LocalProjectionProviderDataAMP< CameraModel >* m_pJacobianData;

	Scalar* m_Ad;
};

class HxPointsGPUTask : public IEvaluatorTask
{
public:

	HxPointsGPUTask( __in accelerator_view dstAccelerator ) :
		m_accelerator( dstAccelerator )
	{
	}

	~HxPointsGPUTask( )
	{
		delete m_pVectorY;
		delete m_pJacobianData;
	}

	void PreInit( __in const TaskInput* pIn ) override
	{
		m_pVectorY = new Vector< Scalar >( pIn->vectorSize );

		m_pointCount = ( uint )min( pIn->maxTaskSize, ( int64 )pIn->pJacobian->GetPointCount( ) );
		LocalProjectionProviderDataAMP< CameraModel >::GetCountsForPointBatch( pIn->pJacobian, 0, m_pointCount, &m_counts );
	}

	int64 Initialize( __in const TaskInput* pIn ) override
	{
		LocalProjectionProviderDataAMP< CameraModel >::CreateForPoints( pIn->pJacobian, 0, &m_counts, m_accelerator, &m_pJacobianData );

		m_Ad = m_pVectorY->Elements( );

		return m_pointCount;
	}

	void Execute( __in const TaskInput* pIn ) override
	{
		constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

		LocalProjectionProviderAMP< CameraModel > localJacobian( m_pJacobianData );
		LocalHessianBlockProviderAMP< CameraModel > localHessianProvider( m_pointCount, localJacobian.GetMaxProjectionCount( ), POINT_PARAM_COUNT );

		LocalHessianMultiplicationEngineAMP< CameraModel > localHessian;
		localHessian.Initialize( pIn->dampening );

		const uint totalCameraParams = cameraParamCount * ( uint )pIn->pJacobian->GetCameraCount( );
		const uint totalPointParams = POINT_PARAM_COUNT * ( uint )pIn->pJacobian->GetPointCount( );
		const uint startIx = 0;

		array< Scalar, 1 > d( totalCameraParams + totalPointParams, pIn->pVectorX, m_accelerator );
		array< Scalar, 1 > Ad( m_pointCount * POINT_PARAM_COUNT, m_Ad, m_accelerator );

		parallel_for_each( m_accelerator, extent< 1 >( m_pointCount ),
			[ localJacobian, localHessianProvider, localHessian, totalCameraParams, totalPointParams, startIx, &d, &Ad ]
		( index< 1 > localPointIx ) restrict( amp )
		{
			constexpr uint pointParamCount = POINT_PARAM_COUNT;

			const Scalar* pCameraD = d.data( );
			const Scalar* pPointD = pCameraD + totalCameraParams;

			Scalar* pAd = Ad.data( ) + localPointIx[0] * pointParamCount;

			localHessian.MultiplyPointRow( &localJacobian, &localHessianProvider, localPointIx[0], totalCameraParams, pCameraD, totalPointParams, pPointD, pAd );
		} );

		m_accelerator.wait( );

		copy( Ad, m_Ad );
	}

protected:

	accelerator_view m_accelerator;

	Vector< Scalar >* m_pVectorY;

	uint m_pointCount;

	LocalProjectionProviderDataCountsAMP m_counts;
	LocalProjectionProviderDataAMP< CameraModel >* m_pJacobianData;

	Scalar* m_Ad;
};
