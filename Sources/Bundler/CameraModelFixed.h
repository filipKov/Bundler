#pragma once

namespace Bundler { namespace CameraModels {

	class CameraModelFixed : public Camera
	{
	public:
	
		static constexpr const uint cameraParameterCount = 0;
	
		static constexpr const uint totalParamCount = cameraParameterCount + POINT_PARAM_COUNT;
	
		static constexpr const uint pointParamStartIx = cameraParameterCount;
	
	public:
	
		CameraModelFixed( ) __CPU_ONLY
		{
		}
	
		CameraModelFixed( __in const Camera* pCamera ) __CPU_ONLY
		{
			Initialize( pCamera );
		}
	
		void Initialize( __in const Camera* pCamera ) __CPU_ONLY
		{
			ShallowCopy< Camera >( pCamera, 1, this );
		}
	
		CameraModelFixed& operator=( __in const CameraModelFixed& src ) __CPU_ONLY = default;
	
		void SetCopy( __in const CameraModelFixed* pSource ) __CPU_ONLY
		{
			*this = *pSource;
		}

		void CopyToCamera( __out Camera* pCamera )
		{
			ShallowCopy< Camera >( this, 1, pCamera );
		}
	
		void UpdateCamera( __in_ecount( cameraParameterCount ) const Scalar* pDeltaParams ) __CPU_ONLY
		{
			UNREFERENCED_PARAMETER( pDeltaParams );
		}
	
		void ResetLastUpdate( __in_ecount( cameraParameterCount ) const Scalar* pDeltaParams ) __CPU_ONLY
		{
			UNREFERENCED_PARAMETER( pDeltaParams );
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
	
			ELEMENT( pt1, 2 ) = Scalar( 1.0 ) / ELEMENT( pt1, 2 );
			ELEMENT( pResiduals, 0 ) = ( ELEMENT( pt1, 0 ) * ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 0 );
			ELEMENT( pResiduals, 1 ) = ( ELEMENT( pt1, 1 ) * ELEMENT( pt1, 2 ) ) - ELEMENT( pFeatureCoords, 1 );
		}
	
	protected:
	
		void RotatePoint(
			__in_ecount( 3 ) const DScalar< totalParamCount >* pPoint,
			__out_ecount( 3 ) DScalar< totalParamCount >* pRotatedPoint ) const __GPU
		{
			M33MulV3( r.Elements( ), pPoint, pRotatedPoint );
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
	
	};

} }
