#pragma once

namespace Bundler { namespace CameraModels {

	template < uint maxRotations >
	class CameraModelAMP6DoF : public Camera
	{
	public:
	
		static constexpr const uint rotationParamStartIx = 0;
		
		static constexpr const uint translationParamStartIx = RodriguesRotation::rotationParameterCount;
		
		static constexpr const uint cameraParameterCount = RodriguesRotation::rotationParameterCount + 3;

		static constexpr const uint totalParamCount = cameraParameterCount + POINT_PARAM_COUNT;

		static constexpr const uint pointParamStartIx = cameraParameterCount;

	public:
	
		CameraModelAMP6DoF( ) __CPU_ONLY
		{
			m_currentRotationCount = 0;
		}

		CameraModelAMP6DoF( __in const Camera* pCamera ) __CPU_ONLY
		{
			Initialize( pCamera );
		}

		void Initialize( __in Camera* pCamera ) __CPU_ONLY
		{
			ShallowCopy< Camera >( pCamera, 1, this );
			m_currentRotationCount = 0;
	
			AddInitialRotation( r.Elements( ) );
		}

		CameraModelAMP6DoF< maxRotations >& operator=( __in const CameraModelAMP6DoF< maxRotations >& src ) __CPU_ONLY
		{
			ShallowCopy( &src, 1, this );
			return *this;
		}

		void SetCopy( __in const CameraModelAMP6DoF< maxRotations >* pSource ) __CPU_ONLY
		{
			*this = *pSource;
		}
	
		void UpdateCamera( __in_ecount( cameraParamCount ) const Scalar* pDeltaParams ) __CPU_ONLY
		{
			AddRotation< false >( ELEMENT( pDeltaParams, rotationParamStartIx + 0 ), pDeltaParams + rotationParamStartIx + 1 );
	
			V3AddV3( t.Elements( ), pDeltaParams + translationParamStartIx, t.Elements( ) );
		}
	
		void ResetLastUpdate( __in_ecount( cameraParamCount ) const Scalar* pDeltaParams ) __CPU_ONLY
		{
			m_currentRotationCount--;
			V3SubV3( t.Elements( ), pDeltaParams + translationParamStartIx, t.Elements( ) );
		}
	
		void ProjectPoint(
			__in_ecount( 3 ) const Scalar* pPointCoords,
			__in_ecount( 2 ) const Scalar* pFeatureCoords,
			__out_ecount( 2 ) DScalar< totalParamCount >* pResiduals ) const __GPU
		{
			DScalar< totalParamCount > pt1[3] =
			{
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 0 ), cameraParameterCount + 0 ),
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 1 ), cameraParameterCount + 1 ),
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 2 ), cameraParameterCount + 2 ),
			};
	
			DScalar< totalParamCount > pt2[3];
			RotatePoint( pt1, pt2 );
			TranslatePoint( pt2 );
			MultiplyByCalibration( pt2, pt1 );
	
			ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 0 ) / ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 0 );
			ELEMENT( pResiduals, 1 ) = ( ELEMENT( pt1, 1 ) / ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 1 );
		}
	
	protected:
	
		void RotatePoint(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pRotatedPoint ) const __GPU
		{
			DScalar< totalParamCount > tempPoint[3];
			Containers::ArrayUtils< DScalar< totalParamCount > >::Copy< 3 >( pPoint, tempPoint );
	
			auto pt1 = tempPoint;
			auto pt2 = pRotatedPoint;
	
			for ( uint i = 0; i < m_currentRotationCount; i++ )
			{
				M33MulV3( m_incrementalRotations[i].Elements( ), pt1, pt2 );
	
				auto tmp = pt1;
				pt1 = pt2;
				pt2 = tmp;
			}
	
			if ( m_currentRotationCount % 2 == 0 )
			{
				Containers::ArrayUtils< DScalar< totalParamCount > >::Copy< 3 >( tempPoint, pRotatedPoint );
			}
		}
	
		void TranslatePoint( __inout_ecount( 3 ) DScalar< totalParamCount >* pPoint ) const __GPU
		{
			const Scalar* pT = t.Elements( );
	
			DScalar< totalParamCount > translation[3] =
			{
				DScalar< totalParamCount >( ELEMENT( pT, 0 ), translationParamStartIx + 0 ),
				DScalar< totalParamCount >( ELEMENT( pT, 1 ), translationParamStartIx + 1 ),
				DScalar< totalParamCount >( ELEMENT( pT, 2 ), translationParamStartIx + 2 ),
			};
	
			V3AddV3( pPoint, translation, pPoint );
		}
	
		void MultiplyByCalibration(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pCalibratedPoint ) const __GPU
		{
			ELEMENT( pCalibratedPoint, 0 ) = ELEMENT( pPoint, 0 ) * focalLength;
			ELEMENT( pCalibratedPoint, 1 ) = ELEMENT( pPoint, 1 ) * focalLength;
			ELEMENT( pCalibratedPoint, 2 ) = ELEMENT( pPoint, 2 );
		}
	
		void AddInitialRotation( __in_ecount( 9 ) const Scalar* pRotMatrix ) __CPU_ONLY
		{
			Scalar angle;
			Scalar axis[3];
			RodriguesRotation::GetFromRotationMatrix( pRotMatrix, &angle, axis );
	
			AddRotation< true >( angle, axis );
		}
	
		template < bool isFirst = false >
		void AddRotation( __in const Scalar angle, __in_ecount( 3 ) const Scalar* pAxis ) __CPU_ONLY
		{
			RodriguesRotation::GetRotation< totalParamCount, rotationParamStartIx, isFirst >( angle, pAxis, m_incrementalRotations[m_currentRotationCount].Elements( ) );
			m_currentRotationCount++;
		}
	
	protected:

		DMatrix3x3< totalParamCount > m_incrementalRotations[maxRotations];
		uint m_currentRotationCount;
	
	};

} }
