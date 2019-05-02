#pragma once

namespace Bundler { namespace CameraModels {

	template < uint maxRotations >
	class CameraModel3DoF_Rotation : public Camera
	{
	public:
	
		static constexpr const uint cameraParameterCount = RodriguesRotation::rotationParameterCount;
	
		static constexpr const uint totalParamCount = cameraParameterCount + POINT_PARAM_COUNT;
	
		static constexpr const uint pointParamStartIx = cameraParameterCount;
	
	public:
	
		CameraModel3DoF_Rotation( ) __CPU_ONLY
		{
			m_currentRotationCount = 0;
		}
	
		CameraModel3DoF_Rotation( __in const Camera* pCamera ) __CPU_ONLY
		{
			Initialize( pCamera );
		}
	
		void Initialize( __in const Camera* pCamera ) __CPU_ONLY
		{
			ShallowCopy< Camera >( pCamera, 1, this );
			m_currentRotationCount = 0;
	
			AddInitialRotation( r.Elements( ) );
		}
	
		CameraModel3DoF_Rotation< maxRotations >& operator=( __in const CameraModel3DoF_Rotation< maxRotations >& src ) __CPU_ONLY = default;
	
		void SetCopy( __in const CameraModel3DoF_Rotation< maxRotations >* pSource ) __CPU_ONLY
		{
			*this = *pSource;
		}

		void CopyToCamera( __out Camera* pCamera )
		{
			ExtractRotation( r.Elements( ) );
			ShallowCopy< Camera >( this, 1, pCamera );
		}
	
		void UpdateCamera( __in_ecount( cameraParameterCount ) const Scalar* pDeltaParams ) __CPU_ONLY
		{
			AddRotation< false >( *pDeltaParams, pDeltaParams + 1 );
		}
	
		void ResetLastUpdate( __in_ecount( cameraParameterCount ) const Scalar* pDeltaParams ) __CPU_ONLY
		{
			m_currentRotationCount--;
		}
	
		void ProjectPoint(
			__in_ecount( 3 ) const Scalar* pPointCoords,
			__in_ecount( 2 ) const Scalar* pFeatureCoords,
			__out_ecount( 2 ) DScalar< totalParamCount >* pResiduals ) const __GPU
		{
			DScalar< totalParamCount > pt1[3] =
			{
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 0 ), pointParamStartIx + 0 ),
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 1 ), pointParamStartIx + 1 ),
				DScalar< totalParamCount >( ELEMENT( pPointCoords, 2 ), pointParamStartIx + 2 ),
			};
	
			DScalar< totalParamCount > pt2[3];
			RotatePoint( pt1, pt2 );
			TranslatePoint( pt2 );
			MultiplyByCalibration( pt2, pt1 );
	
			ELEMENT( pt1, 2 ) = Scalar( 1.0 ) / ELEMENT( pt1, 2 );
			ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 0 ) * ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 0 );
			ELEMENT( pResiduals, 1 ) = ( ELEMENT( pt1, 1 ) * ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 1 );
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
			V3AddV3( pPoint, t.Elements(), pPoint );
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
			RodriguesRotation::GetRotation< totalParamCount, 0, isFirst >( angle, pAxis, m_incrementalRotations[m_currentRotationCount].Elements( ) );
			m_currentRotationCount++;
		}
	
		void ExtractRotation( __out_ecount( 9 ) Scalar* pRot )
		{
			DScalar< totalParamCount > tmp1[9];
			DScalar< totalParamCount > tmp2[9];

			ShallowCopy( m_incrementalRotations[0].Elements( ), 9, tmp1 );
			for ( uint i = 1; i < m_incrementalRotations; i++ )
			{
				M33MulM33( tmp1, m_incrementalRotations[i].Elements( ), tmp2 );
				ShallowCopy( tmp2, 9, tmp1 );
			}

			for ( uint i = 0; i < 9; i++ )
			{
				ELEMENT( pRot, i ) = ELEMENT( tmp1, i ).GetFx( );
			}
		}

	protected:
	
		DMatrix3x3< totalParamCount > m_incrementalRotations[maxRotations];
		uint m_currentRotationCount;
	
	};

} }
