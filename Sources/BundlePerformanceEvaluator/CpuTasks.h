#pragma once

class HxCamerasCPUTask : public IEvaluatorTask
{
public:

	~HxCamerasCPUTask( )
	{
		delete m_pVectorY;
	}

	void PreInit( __in const TaskInput* pIn ) override
	{
		m_pVectorY = new Vector< Scalar >( pIn->vectorSize );
	}

	int64 Initialize( __in const TaskInput* pIn ) override
	{
		constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

		m_cameraCount = ( uint )min( pIn->maxTaskSize, ( int64 )pIn->pJacobian->GetCameraCount() );

		localJacobian.InitializeForCameras( pIn->pJacobian, 0, m_cameraCount );
		localHessian.Initialize( &localJacobian, pIn->dampening );

		totalCameraParams = cameraParamCount * ( uint )pIn->pJacobian->GetCameraCount( );
		pCameraD = pIn->pVectorX;

		totalPointParams = POINT_PARAM_COUNT * ( uint )pIn->pJacobian->GetPointCount( );
		pPointD = pIn->pVectorX + totalCameraParams;

		pD = Bundler::Utils::GetCameraParamPtr< CameraModel >( 0, pIn->pVectorX );
		m_Ad = m_pVectorY->Elements( );

		return m_cameraCount;
	}

	void Execute( __in const TaskInput* pIn ) override
	{
		UNREFERENCED_PARAMETER( pIn );

		constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

		for ( uint cameraIx = 0; cameraIx < localHessian.GetCameraCount( ); cameraIx++ )
		{
			localHessian.MultiplyCameraRow( cameraIx, totalCameraParams, pCameraD, totalPointParams, pPointD, m_Ad );
			pD += cameraParamCount;
			m_Ad += cameraParamCount;
		}
	}

protected:

	Vector< Scalar >* m_pVectorY;
	
	uint m_cameraCount;

	LocalProjectionProviderCPU< CameraModel > localJacobian;
	LocalHessianMultiplicationEngineCPU< CameraModel > localHessian;

	Scalar* pD;
	Scalar* m_Ad;

	Scalar* pCameraD;
	Scalar* pPointD;

	uint totalCameraParams;
	uint totalPointParams;
};

class HxPointsCPUTask : public IEvaluatorTask
{
public:

	~HxPointsCPUTask( )
	{
		delete m_pVectorY;
	}

	void PreInit( __in const TaskInput* pIn ) override
	{
		m_pVectorY = new Vector< Scalar >( pIn->vectorSize );
	}

	int64 Initialize( __in const TaskInput* pIn ) override
	{
		constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

		m_pointCount = ( uint )min( pIn->maxTaskSize, ( int64 )pIn->pJacobian->GetPointCount( ) );

		localJacobian.InitializeForPoints( pIn->pJacobian, 0, m_pointCount );
		localHessian.Initialize( &localJacobian, pIn->dampening );

		totalCameraParams = cameraParamCount * ( uint )pIn->pJacobian->GetCameraCount( );
		pCameraD = pIn->pVectorX;

		totalPointParams = POINT_PARAM_COUNT * ( uint )pIn->pJacobian->GetPointCount( );
		pPointD = pIn->pVectorX + totalCameraParams;

		pD = Bundler::Utils::GetPointParamPtr< CameraModel >( 0, pIn->pJacobian->GetCameraCount( ), pIn->pVectorX );
		m_Ad = m_pVectorY->Elements( );

		return m_pointCount;
	}

	void Execute( __in const TaskInput* pIn ) override
	{
		UNREFERENCED_PARAMETER( pIn );

		constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;

		for ( uint pointIx = 0; pointIx < localHessian.GetPointCount( ); pointIx++ )
		{
			localHessian.MultiplyPointRow( pointIx, totalCameraParams, pCameraD, totalPointParams, pPointD, m_Ad );

			pD += POINT_PARAM_COUNT;
			m_Ad += POINT_PARAM_COUNT;
		}
	}

protected:

	Vector< Scalar >* m_pVectorY;

	uint m_pointCount;

	LocalProjectionProviderCPU< CameraModel > localJacobian;
	LocalHessianMultiplicationEngineCPU< CameraModel > localHessian;

	Scalar* pD;
	Scalar* m_Ad;

	Scalar* pCameraD;
	Scalar* pPointD;

	uint totalCameraParams;
	uint totalPointParams;
};
