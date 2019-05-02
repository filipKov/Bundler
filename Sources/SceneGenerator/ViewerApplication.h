#pragma once

class ViewerApplication
{
public:

	static ViewerApplication* GetApp();
	static void Release();

	void Initialize( __in int argc, __in char **argv );

	void Start();

	void RenderScene();

	void ReshapeWindow( __in const uint newWidth, __in const uint newHeight );

	void OnMouseEvent( __in const int button, __in const int state, __in const int x, __in const int y );

	void OnMouseDrag( __in const int x, __in const int y );

	void OnKeyPressed( __in const byte key, __in const int x, __in const int y );

	void OnKeyReleased( __in const byte key, __in const int x, __in const int y );

	void ShowPointCloud( __in ScenePointCloud* pPointCloud );

	void ShowMesh( __in SceneMesh* pMesh );

	void GetNewTexture( __out Texture** ppTexture );

	ProgressBar& GetProgressBar();

protected:

	ViewerApplication();
	~ViewerApplication();

	void Clear();

	void InitializeOpenGL( __in int argc, __in char **argv );

	void SetupCallbacks();

	void SetupWindow();

	void LoadDefaults();

	void LoadDefaultProperties();

	void LoadDefaultRendererComponents();

	void LoadDefaultScene();

	void LoadDefaultLights();

	void LoadDefaultObjects();

	void LoadDefaultCamera();

	void PrintDebugInfo();

	void SetRenderable( __in const size_t renderableIx );


protected:

	static ViewerApplication* m_pApp;

	// ------------------------------
	//	Subsystems
	// ------------------------------
	ProgressBar mProgressBar;
	
	// Rendering
	GlRenderer mRenderer;
	RendererComponents mRendererComponents;
	
	TextureCache mTextureCache;

	Timer mTimer;
	size_t mFrameCount;
	LARGE_INTEGER mLastTimePoint;


	// ----------------------
	//	Window
	// ----------------------
	Viewport mMainWindow; // { 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT };

	// ----------------------
	//	Scene
	// ----------------------
	Scene mScene;
	CameraController mCameraController;

	std::mutex mDataLock;
	bool mInitialized;

};

