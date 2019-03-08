#pragma once

namespace Bundler { namespace Structure {

	class BundleStructureMapping 
	{
	public:

		void Initialize(
			__in const size_t cameraCount,
			__in const size_t pointCount,
			__in const uint projectionCount,
			__in_ecount( projectionCount ) const Projection* pProjections )
		{
			m_cameraToProjectionMapping.Allocate( cameraCount );
			m_pointToProjectionMapping.Allocate( pointCount );

			const Projection* pProjection = pProjections;
			for ( size_t projI = 0; projI < projectionCount; projI++ )
			{
				m_cameraToProjectionMapping[ pProjection->cameraIndex ].Add( projI );
				m_pointToProjectionMapping[ pProjection->pointIndex ].Add( projI );

				pProjection++;
			}
		}

		size_t GetCameraProjectionCount( __in const size_t cameraIx ) const
		{
			return m_cameraToProjectionMapping[ cameraIx ].Length();
		}

		size_t GetCameraProjectionIndex( __in const size_t cameraIx, __in const size_t ix ) const
		{
			return m_cameraToProjectionMapping[ cameraIx ][ ix ];
		}

		size_t GetPointProjectionCount( __in const size_t pointIx ) const
		{
			return m_pointToProjectionMapping[ pointIx ].Length();
		}

		size_t GetPointProjectionIndex( __in const size_t pointIx, __in const size_t ix ) const
		{
			return m_pointToProjectionMapping[ pointIx ][ ix ];
		}

	protected:

		Containers::Buffer< Containers::PagedVector< size_t, 13 > > m_cameraToProjectionMapping;
		Containers::Buffer< Containers::PagedVector< size_t, 5 > > m_pointToProjectionMapping;

	};

} }