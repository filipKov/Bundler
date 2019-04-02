#pragma once

namespace Bundler { namespace Async {

	template < class CameraModel >
	class HessianCameraRowMultiplicationTask : ITask
	{
	public:
	
		void Initialize( __in const int64 memoryLimitKB )
		{
			m_memoryLimitKB = memoryLimitKB;
		}
	
		void InitializeWorkload ( 
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint cameraStartIx,
			__in const Scalar dampeningFactor,
			__in const uint parameterCount,
			__in_ecount( parameterCount ) const Scalar* pX,
			__out_ecount( parameterCount ) Scalar* pDestination )
		{
			m_jacobian.InitializeForCameras( pJacobian, cameraStartIx, m_memoryLimitKB );
	
			m_dampeningFactor = dampeningFactor;
	
			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
	
			m_pCameraX = Utils::GetCameraParamPtr< CameraModel >( 0, pX );
			m_pPointX = Utils::GetPointParamPtr< CameraModel >( 0, cameraCount, pX );
	
			m_cameraParameterCount = uint( m_pPointX - m_pCameraX );
			m_pointParameterCount = parameterCount - m_cameraParameterCount;
	
			m_pDestination = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pDestination );
		}
	
		const LocalProjectionProvider< CameraModel >& GetJacobian( ) const
		{
			return m_jacobian;
		}
	
		void Execute( ) override
		{
			LocalHessianMultiplicationEngine< CameraModel > hessian;
			hessian.Initialize( &m_jacobian, m_dampeningFactor );
	
			for ( uint cameraIx = 0; cameraIx < hessian.GetCameraCount( ); cameraIx++ )
			{
				hessian.MultiplyCameraRow(
					cameraIx,
					m_cameraParameterCount, m_pCameraX,
					m_pointParameterCount, m_pPointX,
					m_pDestination );
	
				m_pDestination += CameraModel::cameraParameterCount;
			}
		}
	
	protected:
	
		int64 m_memoryLimitKB;
	
		LocalProjectionProvider< CameraModel > m_jacobian;
	
		Scalar m_dampeningFactor;
	
		uint m_cameraParameterCount;
		const Scalar* m_pCameraX;
	
		uint m_pointParameterCount;
		const Scalar* m_pPointX;
	
		Scalar* m_pDestination;
	};

} }