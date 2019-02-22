#pragma once

namespace Bundler {

	template < class CameraModel >
	class IJacobianProvider {
	public:

		virtual uint GetCameraCount() const = 0;

		virtual uint GetCameraProjectionCount( __in const uint cameraIx ) const = 0;

		virtual uint GetCameraProjectionIndex( __in const uint cameraIx, __in const uint ix ) const = 0;

		virtual void GetCameraProjectionIndices( __in const uint cameraIx, __inout uint* pCount, __out_ecount( *pCount ) uint* pIndices ) const = 0;


		virtual uint GetPointCount() const = 0;

		virtual uint GetPointProjectionCount( __in const uint pointIx ) const = 0;

		virtual uint GetPointProjectionIndex( __in const uint pointIx, __in const uint ix ) const = 0;

		virtual void GetPointProjectionIndices( __in const uint pointIx, __inout uint* pCount, __out_ecount( *pCount ) uint* pIndices ) const = 0;


		virtual void GetProjection(
			__in const uint projectionIx,
			__out uint* pCameraIndex,
			__out uint* pPointIndex,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const = 0;

		virtual void ConvertToBlocks(
			__in_ecount( 2 ) const DScalar< CameraModel::totalParamCount >* pResiduals,
			__out JacobianCameraBlock< CameraModel::totalParamCount >* pCameraBlock,
			__out JacobianPointBlock* pPointBlock ) const = 0;

	};

}