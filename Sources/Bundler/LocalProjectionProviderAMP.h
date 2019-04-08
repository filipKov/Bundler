#pragma once

namespace Bundler {

	template < class CameraModel >
	class LocalProjectionProviderAMP
	{
	public:

		__forceinline uint GetCameraCount( ) const __GPU;

		__forceinline uint GetCameraProjectionCount( __in const uint localCameraIx ) const __GPU;

		__forceinline uint GetCameraProjectionIndex( __in const uint localCameraIx, __in const uint ix ) const __GPU;

		__forceinline uint GetCameraIndex( __in const uint projectionIx ) const __GPU;

		__forceinline uint GetGlobalCameraIndex( __in const uint localCameraIx ) const __GPU;


		__forceinline uint GetPointCount( ) const __GPU;

		__forceinline uint GetPointProjectionCount( __in const uint localPointIx ) const __GPU;

		__forceinline uint GetPointProjectionIndex( __in const uint localPointIx, __in const uint ix ) const __GPU;

		__forceinline uint GetPointIndex( __in const uint projectionIx ) const __GPU;

		__forceinline uint GetGlobalPointIndex( __in const uint localPointIx ) const __GPU;


		template < bool getCameraBlock, bool getPointBlock, bool getResiduals >
		__forceinline void GetProjectionBlock(
			__in const size_t projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const __GPU;

	protected:

		// Bundle
		array_view< Camera, 1 > m_cameras;
		array_view< CameraModel, 1 > m_cameraModels;
		array_view< Vector3, 1 > m_points;
		array_view< Projection, 1 > m_projections;

		// Mapping



	};

}
