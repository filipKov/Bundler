#pragma once

namespace Bundler {

	namespace Structure {
		struct LocalBundleStructureMapping
		{
			uint startIndex;
			uint projectionCount;
		};
	}

	// Forward declarations for local provider factory strategies
	template < class CameraModel >
	class MemoryLimitedPreciseStrategy;

	template < class CameraModel >
	class MemoryLimitedPreciseFastStrategy;
	// end

	template < class CameraModel >
	class LocalProjectionProvider
	{
	public:

		uint GetCameraCount( ) const
		{
			return ( uint )m_cameras.Length( );
		}

		uint GetCameraProjectionCount( __in const uint cameraIx ) const
		{
			return m_mapping[cameraIx].projectionCount;
		}

		uint GetCameraProjectionIndex( __in const uint cameraIx, __in const uint ix ) const
		{
			return m_mapping[cameraIx].startIndex + ix;
		}

		uint GetCameraIndex( __in const uint projectionIx ) const
		{
			return m_projections[projectionIx].cameraIndex;
		}

		uint GetGlobalCameraIndex( __in const uint localCameraIx ) const
		{
			return m_cameraGlobalMapping[localCameraIx];
		}


		uint GetPointCount( ) const
		{
			return ( uint )m_points.Length( );
		}

		uint GetPointProjectionCount( __in const uint pointIx ) const
		{
			return m_mapping[pointIx].projectionCount;
		}

		uint GetPointProjectionIndex( __in const uint pointIx, __in const uint ix ) const
		{
			return m_mapping[pointIx].startIndex + ix;
		}

		uint GetPointIndex( __in const uint projectionIx ) const
		{
			return m_projections[projectionIx].pointIndex;
		}

		uint GetGlobalPointIndex( __in const uint localPointIx ) const
		{
			return m_pointGlobalMapping[localPointIx];
		}


		uint GetProjectionCount( ) const
		{
			return ( uint )m_projections.Length( );
		}

		template < bool getCameraBlock, bool getPointBlock, bool getResiduals >
		void GetProjectionBlock(
			__in const size_t projectionIx,
			__out_ecount_opt( 2 * CameraModel::cameraParameterCount ) Scalar* pCameraBlock,
			__out_ecount_opt( 2 * POINT_PARAM_COUNT ) Scalar* pPointBlock,
			__out_ecount_opt( 2 ) Scalar* pResiduals ) const
		{
			DScalar< CameraModel::totalParamCount > residuals[2];
			GetProjection( projectionIx, residuals );

			if ( getCameraBlock )
			{
				_ASSERT_EXPR( pCameraBlock != NULL, "Camera block must not be null" );
				ExtractCameraParameters( residuals + 0, pCameraBlock + 0 );
				ExtractCameraParameters( residuals + 1, pCameraBlock + CameraModel::cameraParameterCount );
			}

			if ( getPointBlock )
			{
				_ASSERT_EXPR( pPointBlock != NULL, "Point block must not be null" );
				ExtractPointParameters( residuals + 0, pPointBlock + 0 );
				ExtractPointParameters( residuals + 1, pPointBlock + POINT_PARAM_COUNT );
			}

			if ( getResiduals )
			{
				_ASSERT_EXPR( pResiduals != NULL, "Residual block must not be null" );
				ELEMENT( pResiduals, 0 ) = ELEMENT( residuals, 0 ).GetFx( );
				ELEMENT( pResiduals, 1 ) = ELEMENT( residuals, 1 ).GetFx( );
			}
		}

	protected:

		void GetProjection(
			__in const size_t projectionIx,
			__out_ecount( 2 ) DScalar< CameraModel::totalParamCount >* pResiduals ) const
		{
			const Projection* pProjection = m_projections.Data() + projectionIx;

			const uint cameraIndex = pProjection->cameraIndex;
			const uint pointIndex = pProjection->pointIndex;

			m_cameraModels[cameraIndex].ProjectPoint( m_points[pointIndex].Elements( ), pProjection->projectedPoint.Elements( ), pResiduals );
		}

		inline void ExtractCameraParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const
		{
			ShallowCopy< Scalar >(
				pResidual->GetDiff( ).Elements( ),
				CameraModel::cameraParameterCount,
				pDestination );
		}

		inline void ExtractPointParameters(
			__in const DScalar< CameraModel::totalParamCount >* pResidual,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pDestination ) const
		{
			ShallowCopy< Scalar >(
				pResidual->GetDiff( ).Elements( ) + CameraModel::pointParamStartIx,
				POINT_PARAM_COUNT,
				pDestination );
		}

	protected:
		
		Containers::Buffer< Camera > m_cameras;
		Containers::Buffer< CameraModel > m_cameraModels;

		Containers::Buffer< Vector3 > m_points;
		
		Containers::Buffer< Projection > m_projections;

		Containers::Buffer< Structure::LocalBundleStructureMapping > m_mapping;
		
		Containers::Buffer< uint > m_cameraGlobalMapping;
		Containers::Buffer< uint > m_pointGlobalMapping;

		friend class MemoryLimitedPreciseStrategy< CameraModel >;
		friend class MemoryLimitedPreciseFastStrategy< CameraModel >;
	};

}