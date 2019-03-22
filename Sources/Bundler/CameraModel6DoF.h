#pragma once

namespace Bundler { namespace CameraModels {

	template < uint maxRotations >
	class CameraModel6DoF: public BasicCameraModel< RodriguesRotation::rotationParameterCount + 3 >
	{
	protected:

		static constexpr const uint rotationParamStartIx = 0;
		static constexpr const uint translationParamStartIx = RodriguesRotation::rotationParameterCount;

	public:

		void Initialize( __in Camera* pCamera ) override 
		{
			m_pCamera = pCamera;
			m_currentRotationCount = 0;

			AddInitialRotation( pCamera->r.Elements() );
		}

		void UpdateCamera( __in_ecount( cameraParamCount ) const Scalar* pDeltaParams ) override
		{
			AddRotation< false >( ELEMENT( pDeltaParams, rotationParamStartIx + 0 ), pDeltaParams + rotationParamStartIx + 1 );

			V3AddV3( m_pCamera->t.Elements(), pDeltaParams + translationParamStartIx, m_pCamera->t.Elements() );
		}

		void ResetLastUpdate( __in_ecount( cameraParamCount ) const Scalar* pDeltaParams ) override
		{
			m_currentRotationCount--;
			V3SubV3( m_pCamera->t.Elements( ), pDeltaParams + translationParamStartIx, m_pCamera->t.Elements( ) );
		}

	protected:

		void RotatePoint(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pRotatedPoint ) const override 
		{
			DScalar< totalParamCount > tempPoint[ 3 ];
			ShallowCopy( pPoint, 3, tempPoint );

			auto pt1 = tempPoint;
			auto pt2 = pRotatedPoint;

			for ( uint i = 0; i < m_currentRotationCount; i++ ) {
				M33MulV3( m_incrementalRotations[ i ].Elements(), pt1, pt2 );

				auto tmp = pt1;
				pt1 = pt2;
				pt2 = tmp;
			}
			
			if ( m_currentRotationCount % 2 == 0 ) {
				ShallowCopy( tempPoint, 3, pRotatedPoint );
			}
		}

		void TranslatePoint( __inout_ecount( 3 ) DScalar< totalParamCount >* pPoint ) const override 
		{
			const Scalar* pT = m_pCamera->t.Elements();

			DScalar< totalParamCount > translation[ 3 ] =
			{
				DScalar< totalParamCount >( ELEMENT( pT, 0 ), translationParamStartIx + 0 ),
				DScalar< totalParamCount >( ELEMENT( pT, 1 ), translationParamStartIx + 1 ),
				DScalar< totalParamCount >( ELEMENT( pT, 2 ), translationParamStartIx + 2 ),
			};

			V3AddV3( pPoint, translation, pPoint );
		}

		void MultiplyByCalibration(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pCalibratedPoint ) const override 
		{
			const Scalar focalLength = m_pCamera->focalLength;

			ELEMENT( pCalibratedPoint, 0 ) = ELEMENT( pPoint, 0 ) * focalLength;
			ELEMENT( pCalibratedPoint, 1 ) = ELEMENT( pPoint, 1 ) * focalLength;
			ELEMENT( pCalibratedPoint, 2 ) = ELEMENT( pPoint, 2 );
		}

		void AddInitialRotation( __in_ecount( 9 ) const Scalar* pRotMatrix ) 
		{
			Scalar angle;
			Scalar axis[ 3 ];
			RodriguesRotation::GetFromRotationMatrix( pRotMatrix, &angle, axis );

			AddRotation< true >( angle, axis );
		}

		template < bool isFirst = false >
		void AddRotation( __in const Scalar angle, __in_ecount( 3 ) const Scalar* pAxis ) {
			RodriguesRotation::GetRotation< totalParamCount, rotationParamStartIx, isFirst >( angle, pAxis, m_incrementalRotations[ m_currentRotationCount ].Elements() );
			m_currentRotationCount++;
		}

	protected:

		DMatrix3x3< totalParamCount > m_incrementalRotations[ maxRotations ];
		uint m_currentRotationCount;

	};

} }

