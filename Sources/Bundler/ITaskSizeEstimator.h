#pragma once

namespace Bundler { namespace Async {

	template < class CameraModel >
	class ITaskSizeEstimator
	{
	public:

		virtual uint EsimateCameraTaskSize(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint startIx,
			__in const uint bufferCountForCamera,
			__in const uint bufferCountForPoint,
			__in const size_t memoryLimitKB ) = 0;
		// {
		// 	const size_t costForCamera = bufferCountForCamera * CameraModel::cameraParameterCount * sizeof( Scalar );
		// 	const size_t costForPoint = bufferCountForPoint * POINT_PARAM_COUNT * sizeof( Scalar );
		// 
		// }


		virtual uint EsimatePointTaskSize(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__in const uint startIx,
			__in const uint bufferCountForCamera,
			__in const uint bufferCountForPoint,
			__in const size_t memoryLimitKB ) = 0;

	};

} }
