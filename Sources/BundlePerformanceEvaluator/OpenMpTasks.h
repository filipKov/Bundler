#pragma once

class HxCamerasOMPTask : public IEvaluatorTask
{
public:

	~HxCamerasOMPTask( )
	{
		delete m_pVectorY;
	}

	void PreInit( __in const TaskInput* pIn ) override
	{
		m_pVectorY = new Vector< Scalar >( pIn->vectorSize );
	}

	int64 Initialize( __in const TaskInput* pIn ) override
	{
		m_hxEngine.Initialize( pIn->pJacobian, pIn->dampening );
		m_cameraCount = min( pIn->maxTaskSize, (int64)m_hxEngine.GetCameraCount( ) );

		return m_cameraCount;
	}

	void Execute( __in const TaskInput* pIn ) override
	{
	#pragma omp parallel
		{
		#pragma omp for nowait
			for ( int64 cameraIx = 0; cameraIx < m_cameraCount; cameraIx++ )
			{
				Scalar* pYDestination = Bundler::Utils::GetCameraParamPtr< CameraModel >( cameraIx, m_pVectorY->Elements( ) );
				m_hxEngine.MultiplyCameraRow( cameraIx, pIn->vectorSize, pIn->pVectorX, pYDestination );
			}
		}
	}

protected:

	Vector< Scalar >* m_pVectorY;
	HessianMultiplicationEngine< CameraModel > m_hxEngine;
	int64 m_cameraCount;

};

class HxPointsOMPTask : public IEvaluatorTask
{
public:

	~HxPointsOMPTask( )
	{
		delete m_pVectorY;
	}

	void PreInit( __in const TaskInput* pIn ) override
	{
		m_pVectorY = new Vector< Scalar >( pIn->vectorSize );
	}

	int64 Initialize( __in const TaskInput* pIn ) override
	{
		m_hxEngine.Initialize( pIn->pJacobian, pIn->dampening );
		m_pointCount = min( pIn->maxTaskSize, ( int64 )m_hxEngine.GetPointCount( ) );

		return m_pointCount;
	}

	void Execute( __in const TaskInput* pIn ) override
	{
		size_t cameraCount = m_hxEngine.GetCameraCount( );

	#pragma omp parallel
		{
		#pragma omp for
			for ( int64 pointIx = 0; pointIx < m_pointCount; pointIx++ )
			{
				Scalar* pYDestination = Bundler::Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, m_pVectorY->Elements( ) );
				m_hxEngine.MultiplyPointRow( pointIx, pIn->vectorSize, pIn->pVectorX, pYDestination );
			}
		}
	}

protected:

	Vector< Scalar >* m_pVectorY;
	HessianMultiplicationEngine< CameraModel > m_hxEngine;
	int64 m_pointCount;

};
