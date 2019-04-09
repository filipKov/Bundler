#pragma once

namespace Bundler { namespace Internal {

	template < class CameraModel >
	class IProjectionProviderData
	{
	public:

		virtual const CameraModel* GetCamera( __in const uint cameraIx ) const = 0;

		virtual const Vector3* GetPoint( __in const uint pointIx ) const = 0;

		virtual const Projection* GetProjection( __in const uint projectionIx ) const = 0;

	};

} }
