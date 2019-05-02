#include "stdafx.h"

#include "glslang.h"

#include "RenderingLib.h"

#include "Texture.h"
#include "TextureCache.h"

#include "Transformable.h"
#include "SceneObject.h"
#include "ScenePointCloud.h"
#include "SceneMeshPart.h"
#include "SceneMesh.h"
#include "Scene.h"

#include "SceneObjectFactory.h"

#include "Mouse.h"
#include "KeyMacros.h"
#include "CameraController.h"

#include "ProgressBar.h"
#include "ApplicationSectionStrings.h"

#include "ViewerApplication.h"

#define STOPWATCH_FRAME_TIME "frameTime"
#define STOPWATCH_SIMULATION "simulation"

#define MAX_OBJ_SIZE 15.0f

// // ------------------------------
// //	Default Scene
// // ------------------------------
// ObjectLoadParams islands[] = {
// 	ObjectLoadParams { GET_MODEL_PATH( "island1.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) },
// 	ObjectLoadParams { GET_MODEL_PATH( "island2.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) },
// 	ObjectLoadParams { GET_MODEL_PATH( "island3.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) },
// 	ObjectLoadParams { GET_MODEL_PATH( "island4.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) },
// 	ObjectLoadParams { GET_MODEL_PATH( "island5.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) },
// 	ObjectLoadParams { GET_MODEL_PATH( "skyRocks.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) },
// };
// 
MeshLoadParams skybox { GET_MODEL_PATH( "skybox.obj" ), 1, GET_TEXTURE_PATH( "skybox.bmp" ) };

MeshLoadParams testIsland{ GET_MODEL_PATH( "island1.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) };

// 
// ObjectLoadParams clouds { GET_MODEL_PATH( "clouds.obj" ), 1, GET_TEXTURE_PATH( "diffuseTiles.bmp" ) };

// ----------------------------------
//	Workaround OpenGL's C interface
// ----------------------------------
void renderScene(void) {
	ViewerApplication::GetApp()->RenderScene();
}

void reshape(int w,int h)
{ 
	ViewerApplication::GetApp()->ReshapeWindow( w, h );
}

void onMouseEvent( int button, int state, int x, int y ) {
	ViewerApplication::GetApp()->OnMouseEvent( button, state, x, y );
}

void onMouseDrag( int x, int y ) {
	ViewerApplication::GetApp()->OnMouseDrag( x, y );
}

void onKeyPressed( const byte key, const int x, const int y ) {
	ViewerApplication::GetApp()->OnKeyPressed( key, x, y );
}

void onKeyReleased( const byte key, const int x, const int y ) {
	ViewerApplication::GetApp()->OnKeyReleased( key, x, y );
}


// ----------------------------------
//	ViewerApplication methods
// ----------------------------------
ViewerApplication* ViewerApplication::m_pApp = nullptr;

ViewerApplication* ViewerApplication::GetApp() {
	if ( !ViewerApplication::m_pApp ) {
		m_pApp = new ViewerApplication();
	}
	return m_pApp;
}

void ViewerApplication::Release() {
	if ( ViewerApplication::m_pApp ) {
		delete m_pApp;
	}
}

ViewerApplication::ViewerApplication() {
	mInitialized = false;
}

ViewerApplication::~ViewerApplication() 
{
	std::unique_lock< std::mutex > autoLock( mDataLock );
	Clear();
}

void ViewerApplication::Initialize( __in int argc, __in char **argv ) 
{	
	std::unique_lock< std::mutex > autoLock( mDataLock );

	if ( !mInitialized )
	{
		Clear( );

		mProgressBar.BeginSection( AlgID_ViewerInitialization, 1.0f );
		InitializeOpenGL( argc, argv );
		mProgressBar.EndSection( S_OK, NULL );

		printf( "Viewer initialized:\n\tGLEW %s\n\tOpenGL %s\n", glewGetString( GLEW_VERSION ), glGetString( GL_VERSION ) );

		mProgressBar.BeginSection( AlgID_ViewerDefaultSceneLoad, 1.0f );
		LoadDefaults( );
		mProgressBar.EndSection( S_OK, NULL );

		mInitialized = true;
	}
}

void ViewerApplication::Start() {
	TimeGroup& stopwatch = mTimer.GetTimeGroup( STOPWATCH_FRAME_TIME );
	stopwatch.Start();
	mLastTimePoint = stopwatch.lastRecordedPoint;

	TimeGroup& simulationStopwatch = mTimer.GetTimeGroup( STOPWATCH_SIMULATION );
	simulationStopwatch.Start();

	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION );
	glutMainLoop();
}

void ViewerApplication::RenderScene() {
	TimeGroup& stopwatch = mTimer.GetTimeGroup( STOPWATCH_FRAME_TIME );

	stopwatch.SaveElapsed();
	mFrameCount++;
	if ( stopwatch.ElpasedMilliseconds( mLastTimePoint, stopwatch.lastRecordedPoint ) > 1000.0f ) {
		mLastTimePoint = stopwatch.lastRecordedPoint;
		// printf( "1s avg frame time: %fms (%zu frames)\n", stopwatch.GetAverage(), mFrameCount );
		mFrameCount = 0;
		stopwatch.durations.clear();
	}


	TimeGroup& simulationStopwatch = mTimer.GetTimeGroup( STOPWATCH_SIMULATION );
	simulationStopwatch.SaveElapsed();

	std::unique_lock< std::mutex > autoLock( mDataLock );

	float frameTime = (float)simulationStopwatch.GetLast() / 1000.0f;
	mCameraController.Update( frameTime );

	simulationStopwatch.Start();

	stopwatch.Start();

	mRenderer.Render( mScene.camera );
}

void ViewerApplication::ReshapeWindow( __in const uint newWidth, __in const uint newHeight ) {
	mMainWindow.width = newWidth;
	mMainWindow.height = newHeight;

	/* transformation of x and y from normalized device coordinates to window coordinates */
	glViewport( 0, 0, mMainWindow.width, mMainWindow.height );
	float aspectRatio = ( (float)mMainWindow.width ) / mMainWindow.height;
	mScene.camera.SetPerspectiveProjection(
		mMainWindow.cameraFoV,	// FoV in degrees
		aspectRatio,			// aspect ratio
		mMainWindow.clipNear,	// near clipping
		mMainWindow.clipFar		// far clipping
	);

	mRenderer.SetViewport( mMainWindow );
}

void ViewerApplication::OnMouseEvent( __in const int button, __in const int state, __in const int x, __in const int y ) {
	// printf( "button: %d | state: %d | x: %d | y: %d\n", button, state, x, y );

	int modifierBitmask = glutGetModifiers();
	if ( modifierBitmask & GLUT_ACTIVE_CTRL ) {
		mCameraController.SetCtrlActive();
	}
	else {
		mCameraController.SetCrtlInactive();
	}

	const MouseButton btn = (MouseButton)button;
	const MouseState stateInternal = (MouseState)state;

	mCameraController.OnMouseEvent( btn, stateInternal, x, y );
}

void ViewerApplication::OnMouseDrag( __in const int x, __in const int y ) {
	// printf( "pos: [ %d, %d ]\n", x, y );
	mCameraController.OnMouseDrag( x, y );
}

void ViewerApplication::OnKeyPressed( __in const byte key, __in const int x, __in const int y ) {
	uint keyInt = (uint)key;

	switch ( keyInt ) {
		
	case ( KEY_0 ):
			PrintDebugInfo();
			break;

		// Reserved for FPS camera
		case ( KEY_W ):
		case ( KEY_A ):
		case ( KEY_S ):
		case ( KEY_D ):
		case ( KEY_Q ):
		case ( KEY_E ):
			mCameraController.OnKeyDown( keyInt, x, y );
			break;

		case ( KEY_1 ):
			SetRenderable( 0 );
			break;

		case ( KEY_2 ):
			SetRenderable( 1 );
			break;

		case ( KEY_3 ):
			SetRenderable( 2 );
			break;

		case ( KEY_4 ):
			SetRenderable( 3 );
			break;

		case ( KEY_5 ):
			SetRenderable( 4 );
			break;

		case ( KEY_6 ):
			SetRenderable( 5 );
			break;

		case ( KEY_7 ):
			SetRenderable( 6 );
			break;

		case ( KEY_8 ):
			SetRenderable( 7 );
			break;

		case ( KEY_9 ):
			SetRenderable( 8 );
			break;

		case ( KEY_P ):
			mRenderer.SwitchShadowsState();
			break;

		case ( KEY_O ):
			mRenderer.SwitchSSAOState();
			break;

		case ( KEY_G ):
			mScene.pGrid->SwapEnabled();
			break;

		case( KEY_ESC ):
			glutLeaveMainLoop();
			break;
	}
}

void ViewerApplication::OnKeyReleased( __in const byte key, __in const int x, __in const int y ) {
	uint keyInt = (uint)key;
	switch ( keyInt ) {
		case ( KEY_W ):
		case ( KEY_A ):
		case ( KEY_S ):
		case ( KEY_D ):
		case ( KEY_Q ):
		case ( KEY_E ):
			mCameraController.OnKeyUp( keyInt, x, y );
			break;
	}
}

void ViewerApplication::ShowPointCloud( __in ScenePointCloud* pPointCloud ) 
{
	pPointCloud->MoveTo( Vector3f( { 0, 0, 0 } ) );
	pPointCloud->Rotate( 90.0f, Vector3f( { -1, 0, 0 } ) );
	// 
	// const BoundingBox& bbox = pPointCloud->GetBoundingBox();
	// const float objSize = bbox.maxVert.Distance( bbox.minVert );
	// if ( objSize > MAX_OBJ_SIZE ) {
	// 	float scale = MAX_OBJ_SIZE / objSize;
	// 	pPointCloud->Scale( scale );
	// }

	std::unique_lock< std::mutex > autoLock( mDataLock );
	mScene.objects.push_back( pPointCloud );
	SetRenderable( mScene.objects.size( ) - 1 );
}

void ViewerApplication::ShowMesh( __in SceneMesh* pMesh ) 
{
	pMesh->MoveTo( Vector3f( { 0,0,0 } ) );

	pMesh->Rotate( 90.0f, Vector3f( { -1, 0, 0 } ) );
	pMesh->Rotate( 180.0f, Vector3f( { 0, 1, 0 } ) );

	BoundingBox bbox = pMesh->GetBoundingBox();
	const float objSize = bbox.maxVert.Distance( bbox.minVert );
	if ( objSize > MAX_OBJ_SIZE ) {
		float scale = MAX_OBJ_SIZE / objSize;
		pMesh->Scale( scale );
	}

	std::unique_lock< std::mutex > autoLock( mDataLock );

	mScene.objects.push_back( pMesh );
	mRenderer.RegisterRenderable( pMesh );
}

void ViewerApplication::GetNewTexture( __out Texture** ppTexture ) {
	Texture newTex;
	mTextureCache.RegisterItem( "newTex", newTex );
	mTextureCache.Get( "newTex", ppTexture );
}

ProgressBar& ViewerApplication::GetProgressBar() {
	return mProgressBar;
}

void ViewerApplication::Clear() 
{
	mRenderer.Release();
	mTextureCache.Clear();
	mTimer.Clear();
	mFrameCount = 0;
	mLastTimePoint = LARGE_INTEGER();
	mMainWindow = Viewport { 0,0,0,0 };
	mScene.Clear();
	mCameraController.Clear();
	mRendererComponents.Release();
}	

void ViewerApplication::InitializeOpenGL( __in int argc, __in char **argv ) {
	glutInit( &argc, argv );
	SetupWindow();

	GLenum err = glewInit();
	if ( err != GLEW_OK ) {
		char buff[256];
		sprintf_s( buff, "GLEW has encountered error: %s\n", glewGetErrorString( err ) );
		printf( "%s", buff );
		throw Exception( buff );
	}

	SetupCallbacks();
}

void ViewerApplication::SetupCallbacks() {
	glutDisplayFunc( renderScene );
	glutReshapeFunc( reshape );
	glutIdleFunc( renderScene );
	
	glutMouseFunc( onMouseEvent );
	glutMotionFunc( onMouseDrag );
	
	glutKeyboardFunc( onKeyPressed );
	glutKeyboardUpFunc( onKeyReleased );
}

void ViewerApplication::SetupWindow() {
	
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowPosition( 100, 100 );
	glutInitWindowSize( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );
	glutCreateWindow( "ReconstrucThor Viewer" );
	
	mRenderer.Initialize();
}

void ViewerApplication::LoadDefaults() {
	LoadDefaultProperties();

	mProgressBar.OnProgress( 10, 100 );

	LoadDefaultRendererComponents();
	mRenderer.SetComponents( mRendererComponents );

	mProgressBar.OnProgress( 35, 100 );

	LoadDefaultScene();

	mProgressBar.OnProgress( 100, 100 );
}

void ViewerApplication::LoadDefaultProperties() {
	mMainWindow.x = 0;
	mMainWindow.y = 0;
	mMainWindow.width = DEFAULT_WINDOW_WIDTH;
	mMainWindow.height = DEFAULT_WINDOW_HEIGHT;
	mMainWindow.cameraFoV = 45.0f;
	mMainWindow.clipNear = 0.01f;
	mMainWindow.clipFar = 100.0f;
	ReshapeWindow( mMainWindow.width, mMainWindow.height );
}

void ViewerApplication::LoadDefaultRendererComponents() {
	// GLuint diffuseShaderProgram = createShaderProgram1( GET_SHADER_PATH( "flatDiffuse.vert" ), GET_SHADER_PATH( "flatDiffuse.geom" ), GET_SHADER_PATH( "flatDiffuse.frag" ) );
	GLuint diffuseShaderProgram = createShaderProgram1( GET_SHADER_PATH( "flatDiffuse.vert" ), GET_SHADER_PATH( "flatDiffuse.geom" ), GET_SHADER_PATH( "flatDiffuse2.frag" ) );
	
	GLuint pointCloudDiffuseShaderProgram = createShaderProgram( GET_SHADER_PATH( "vertexColor.vert" ), GET_SHADER_PATH( "vertexColor.frag" ) );

	mRendererComponents.pPointCloudShader = new AlbedoPointCloudShader( pointCloudDiffuseShaderProgram, mMainWindow );

	mRendererComponents.pAlbedoShader = new AlbedoShader( diffuseShaderProgram, mMainWindow );
	
	
	// GLuint skyboxShaderProgram = createShaderProgram1( GET_SHADER_PATH( "flatDiffuse.vert" ), GET_SHADER_PATH( "flatDiffuse.geom" ), GET_SHADER_PATH( "skybox.frag" ) );
	// mRendererComponents.pSkyboxShader = new SkyboxShader( skyboxShaderProgram, mMainWindow );
	
	// GLuint ambientOcclusionProgram = createShaderProgram( GET_SHADER_PATH( "ambientOcclusion.vert" ), GET_SHADER_PATH( "ambientOcclusion.frag" ) );
	// AmbientOcclusionShader::Settings ssaoSettings;
	// ssaoSettings.viewport = mMainWindow;
	// ssaoSettings.sampleCount = 64;
	// ssaoSettings.kernelRadius = 0.1f;
	// ssaoSettings.bias = 0.001f;
	// 
	// mRendererComponents.pAmbientOcclusionShader = new AmbientOcclusionShader( ambientOcclusionProgram, ssaoSettings );


	mRendererComponents.bShadowsEnabled = false;
	mRendererComponents.bAoEnabled = false;
}

void ViewerApplication::LoadDefaultScene() {
	LoadDefaultLights();
	LoadDefaultObjects();
	LoadDefaultCamera();
}

void ViewerApplication::LoadDefaultLights() {
	DirectionalLight* pSunlight = (DirectionalLight*)mScene.CreateLight( LIGHT_TYPE::DIRECTIONAL );
	pSunlight->SetDirection( Vector4f( { -5.976418f, -19.618872f, -20.924763f, 0.0f } ) ); // safe ( can see whole scene )
	// pSunlight->SetDirection( Vector4f( {-2.325746f, -8.735946f, -11.957260f, 0.0f } ) );
	pSunlight->SetColor( Vector3f( { 0.788f, 0.886f, 1.0f } ) * 1.0f );
	mRenderer.RegisterLight( pSunlight );

	PointLight* pLight1 = (PointLight*)mScene.CreateLight( LIGHT_TYPE::POINT );
	pLight1->SetPosition( Vector4f( { 0.0f, 100.0f, 0.0f, 1.0f } ) );
	pLight1->SetColor( Vector3f( { 1.0f, 1.0f, 1.0f } ) * 0.2f );
	pLight1->SetAttenuation( 0.0f );
	mRenderer.RegisterLight( pLight1 );
	
	PointLight* pLight2 = (PointLight*)mScene.CreateLight( LIGHT_TYPE::POINT );
	pLight2->SetPosition( Vector4f( { 100.0f, 100.0f, 100.0f, 1.0f } ) );
	pLight2->SetColor( Vector3f( { 1.0f, 1.0f, 1.0f } ) * 0.2f );
	pLight2->SetAttenuation( 0.0f );
	mRenderer.RegisterLight( pLight2 );

	PointLight* pLight3 = (PointLight*)mScene.CreateLight( LIGHT_TYPE::POINT );
	pLight3->SetPosition( Vector4f( { -50.0f, -100.0f, 0.0f, 1.0f } ) );
	pLight3->SetColor( Vector3f( { 1.0f, 1.0f, 1.0f } ) * 0.2f );
	pLight3->SetAttenuation( 0.0f );
	mRenderer.RegisterLight( pLight3 );
}

void ViewerApplication::LoadDefaultObjects() {
	mScene.pGrid = new Grid3D( 10 );
	mRenderer.SetGrid( mScene.pGrid );

	// mScene.pSkybox = LoadMesh( skybox, mTextureCache );
	// mRenderer.RegisterSkybox( mScene.pSkybox->GetRenderable() );
}

void ViewerApplication::LoadDefaultCamera() {
	mScene.camera.SetPosition( Vector3f( { -10 , 5, 0 } ) );
	mScene.camera.SetTarget( Vector3f( { 0,0,0 } ) );
	mCameraController.SetCamera( &mScene.camera );
}

void ViewerApplication::PrintDebugInfo() {
	printf( "--------- Debug Information ---------\n" );

	double avgFrameTime = mTimer.GetTimeGroup( STOPWATCH_FRAME_TIME ).GetAverage();
	printf( "Average frame time: %fms (%.3f FPS)\n", avgFrameTime, 1000.0/avgFrameTime );

	mScene.camera.PrintAvgTimes();
	
	printf( "\n" );
}

void ViewerApplication::SetRenderable( __in const size_t renderableIx )
{
	if ( renderableIx < mScene.objects.size( ) )
	{
		mRenderer.ClearRenderables( );
		mRenderer.RegisterRenderable( mScene.objects[renderableIx]->GetRenderable( ) );
	}
}
