#pragma once

namespace Bundler {

	template < class CameraModel >
	using JacobianCameraBlock = Scalar[ 2 * CameraModel::cameraParameterCount ];

	using JacobianPointBlock = Scalar[ 2 * POINT_PARAM_COUNT ];

	template < class CameraModel >
	class IJacobianProvider {
	public:

		virtual size_t GetCameraCount() const = 0;

		virtual size_t GetCameraProjectionCount( __in const size_t cameraIx ) const = 0;

		virtual size_t GetCameraProjectionIndex( __in const size_t cameraIx, __in const size_t ix ) const = 0;

		// virtual void GetCameraProjectionIndices( __in const size_t cameraIx, __inout size_t* pCount, __out_ecount( *pCount ) size_t* pIndices ) const = 0;


		virtual size_t GetPointCount() const = 0;

		virtual size_t GetPointProjectionCount( __in const size_t pointIx ) const = 0;

		virtual size_t GetPointProjectionIndex( __in const size_t pointIx, __in const size_t ix ) const = 0;

		// virtual void GetPointProjectionIndices( __in const size_t pointIx, __inout size_t* pCount, __out_ecount( *pCount ) size_t* pIndices ) const = 0;


		virtual void GetProjection(
			__in const size_t projectionIx,
			__out size_t* pCameraIndex,
			__out size_t* pPointIndex,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const = 0;

		virtual void ConvertToBlocks(
			__in_ecount( 2 ) const DScalar< CameraModel::totalParamCount >* pResiduals,
			__out_ecount( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock
		) const = 0;

	};

}