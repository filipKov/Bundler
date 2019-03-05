#pragma once

namespace Visualiser {

	struct Scene {
		Utils::Containers::Buffer< RenderablePointCloud > pointClouds;
		Utils::Containers::Buffer< RenderableWireframe > wireframes;
		RenderableWireframe grid;

		Camera viewportCamera;
	};

	class SceneVisualiser {
	public:

		SceneVisualiser( __in const int windowWidth, __in const int windowHeight );

		void Init();

		void Reset();

		void VisualiseBundle( __in const Bundler::Bundle* pBundle, __in_opt const Bundler::BundleAdditionalPayload* pAdditionalData );

		void RenderScene();

		void ReshapeWindow( __in const int newWidth, __in const int newHeight );

		void OnMouseEvent( __in const MouseButton button, __in const MouseState state, __in const int x, __in const int y );

		void OnMouseDrag( __in const int x, __in const int y );

	protected:

		void DrawGrid( __in const Vector3f& startPos, __in const Vector3f& endPos, __in const uint segments );
		
	protected:

		int mWindowWidth;
		int mWindowHeight;

		Scene mScene;
		CameraController mCameraController;

		GlRenderer mRenderer;
	};

}
