#pragma once

namespace Bundler { namespace Async { namespace LinearSolver { namespace Task {

	template < class CameraModel >
	class HessianMulPointRowCPU : public Async::ITask
	{
	public:
	
		void Initialize(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint pointStartIx,
			__in const uint pointCount,
			__in const Scalar diagonalDampening,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY )
		{
			m_localJacobian.InitializeForPoints( pJacobian, pointStartIx, pointCount );
			m_localHessian.Initialize( &m_localJacobian, diagonalDampening );
	
			m_cameraParamCount = ( uint )( pJacobian->GetCameraCount( ) * CameraModel::cameraParameterCount );
			m_pCameraX = pX;
	
			m_pointParamCount = ( uint )( pJacobian->GetPointCount( ) * POINT_PARAM_COUNT );
			m_pPointX = pX + m_cameraParamCount;
	
			m_pDestination = Utils::GetPointParamPtr( pointStartIx,  pJacobian->GetCameraCount(), pY );
		}
	
		void Execute( ) override
		{
			for ( uint localPointIx = 0; localPointIx < m_localHessian.GetPointCount( ); localPointIx++ )
			{
				m_localHessian.MultiplyPointRow(
					localPointIx,
					m_cameraParamCount, m_pCameraX,
					m_pointParamCount, m_pPointX,
					m_pDestination );
	
				m_pDestination += POINT_PARAM_COUNT;
			}
		}
	
	protected:
	
		LocalProjectionProviderCPU< CameraModel > m_localJacobian;
		LocalHessianMultiplicationEngineCPU< CameraModel > m_localHessian;
	
		uint m_cameraParamCount;
		const Scalar* m_pCameraX;
	
		uint m_pointParamCount;
		const Scalar* m_pPointX;
	
		Scalar* m_pDestination;
	};

} } } }
