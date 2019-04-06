#pragma once

namespace Bundler { namespace Async {

	template < class CameraModel >
	class HessianPointRowMultiplicationTask : ITask
	{
	public:
	
		void Initialize( __in const int64 memoryLimitKB )
		{
			m_memoryLimitKB = memoryLimitKB;
		}
	
		void InitializeWorkload (
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint pointStartIx,
			__in const Scalar dampeningFactor,
			__in const uint parameterCount,
			__in_ecount( parameterCount ) const Scalar* pX,
			__out_ecount( parameterCount ) Scalar* pDestination )
		{
			// MemoryLimitedPreciseFastStrategy< CameraModel >::InitializeForPoints( pJacobian, pointStartIx, m_memoryLimitKB, &m_jacobian );
	
			m_dampeningFactor = dampeningFactor;
	
			const uint cameraCount = ( uint )pJacobian->GetCameraCount( );
	
			m_pCameraX = Utils::GetCameraParamPtr< CameraModel >( 0, pX );
			m_pPointX = Utils::GetPointParamPtr< CameraModel >( 0, cameraCount, pX );
	
			m_cameraParameterCount = uint( m_pPointX - m_pCameraX );
			m_pointParameterCount = parameterCount - m_cameraParameterCount;
	
			m_pDestination = Utils::GetPointParamPtr< CameraModel >( pointStartIx, cameraCount, pDestination );
		}
	
		const LocalProjectionProviderCPU< CameraModel >& GetJacobian( ) const
		{
			return m_jacobian;
		}
	
		void Execute( ) override
		{
			LocalHessianMultiplicationEngineCPU< CameraModel > hessian;
			hessian.Initialize( &m_jacobian, m_dampeningFactor );
	
			for ( uint pointIx = 0; pointIx < hessian.GetPointCount( ); pointIx++ )
			{
				hessian.MultiplyPointRow(
					pointIx,
					m_cameraParameterCount, m_pCameraX,
					m_pointParameterCount, m_pPointX,
					m_pDestination );
	
				m_pDestination += CameraModel::cameraParameterCount;
			}
		}
	
	protected:
	
		int64 m_memoryLimitKB;
	
		LocalProjectionProviderCPU< CameraModel > m_jacobian;
	
		Scalar m_dampeningFactor;
	
		uint m_cameraParameterCount;
		const Scalar* m_pCameraX;
	
		uint m_pointParameterCount;
		const Scalar* m_pPointX;
	
		Scalar* m_pDestination;
	};

} }
