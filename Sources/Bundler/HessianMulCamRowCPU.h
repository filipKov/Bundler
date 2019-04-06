#pragma once

namespace Bundler { namespace Async { namespace LinearSolver {namespace Task {

	template < class CameraModel >
	class HessianMulCamRowCPU : public Async::ITask
	{
	public:

		void Initialize(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint cameraStartIx,
			__in const uint cameraCount,
			__in const Scalar diagonalDampening,
			__in const size_t vectorSize,
			__in_ecount( vectorSize ) const Scalar* pX,
			__out_ecount( vectorSize ) Scalar* pY )
		{
			m_localJacobian.InitializeForCameras( pJacobian, cameraStartIx, cameraCount );
			m_localHessian.Initialize( &m_localJacobian, diagonalDampening );

			m_cameraParamCount = ( uint )( pJacobian->GetCameraCount( ) * CameraModel::cameraParameterCount );
			m_pCameraX = pX;

			m_pointParamCount = ( uint )( pJacobian->GetPointCount( ) * POINT_PARAM_COUNT );
			m_pPointX = pX + m_cameraParamCount;

			m_pDestination = Utils::GetCameraParamPtr< CameraModel >( cameraStartIx, pY );
		}

		void Execute( ) override
		{
			for ( uint localCameraIx = 0; localCameraIx < m_localHessian.GetCameraCount( ); localCameraIx++ )
			{
				m_localHessian.MultiplyCameraRow(
					localCameraIx,
					m_cameraParamCount, m_pCameraX,
					m_pointParamCount, m_pPointX,
					m_pDestination );

				m_pDestination += CameraModel::cameraParameterCount;
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
