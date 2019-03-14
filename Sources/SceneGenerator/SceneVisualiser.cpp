#include "stdafx.h"
#include <map>
#include <string>
#include <functional>

#include "GL/glew.h"
#include "GL/glut.h"

#include "AlgebraLib.h"
#include "Camera.h"
#include "RenderableObject.h"
#include "GlRenderer.h"

#include "Mouse.h"
#include "CameraController.h"

#include "SceneVisualiser.h"

using namespace Utils::Containers;

namespace Visualiser {

	SceneVisualiser* pVisualiserInstance;

	void RenderCallback() {
		pVisualiserInstance->RenderScene();
	}

	void ReshapeCallback( int w, int h) {
		pVisualiserInstance->ReshapeWindow( w, h );
	}

	void OnMouseEventCallback( int button, int state, int x, int y ) {
		// printf( "button: %d | state: %d | x: %d | y: %d\n", button, state, x, y );

		const MouseButton btn = (MouseButton)button;
		const MouseState stateInternal = (MouseState)state;
		pVisualiserInstance->OnMouseEvent( btn, stateInternal, x, y );
	}

	void OnMouseDragCallback( int x, int y ) {
		// printf( "pos: [ %d, %d ]\n", x, y );

		pVisualiserInstance->OnMouseDrag( x, y );
	}

	SceneVisualiser::SceneVisualiser( __in const int windowWidth, __in const int windowHeight ) : mCameraController( mScene.viewportCamera ) {
		mWindowWidth = windowWidth;
		mWindowHeight = windowHeight;
		Reset();
	}

	void SceneVisualiser::Init() {
		int dummyArgCnt = 0;
		glutInit( &dummyArgCnt, NULL );
		glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
		glutInitWindowPosition( 100, 100 );
		glutInitWindowSize( mWindowWidth, mWindowHeight );
		glutCreateWindow( "SceneVisualiser" );

		GLenum err = glewInit();
		if ( err != GLEW_OK ) {
			printf( "GLEW has encountered error: %s\n", glewGetErrorString( err ) );
			return;
		}
		
		printf( "Using:\n\tGLEW %s\n\tOpenGL %s\n", glewGetString( GLEW_VERSION ), glGetString( GL_VERSION ) );

		mRenderer.Init();
		mRenderer.SetViewportCamera( mScene.viewportCamera );
		DrawGrid( Vector3f( { -10, 0, -10 } ), Vector3f( { 10, 0, 10 } ), 10 );
		mScene.viewportCamera.SetPosition( Vector3f( { 10,5,10 } ) );
		
		pVisualiserInstance = this;
		glutDisplayFunc( RenderCallback );
		glutReshapeFunc( ReshapeCallback );
		glutIdleFunc( RenderCallback );

		glutMouseFunc( OnMouseEventCallback );
		glutMotionFunc( OnMouseDragCallback );

		// glutKeyboardFunc( ... );

		glEnable( GL_DEPTH_TEST );

		glutMainLoop();
	}

	void SceneVisualiser::Reset() {
		mRenderer.ClearObjects();
		mScene.pointClouds = Buffer< RenderablePointCloud >();
		mScene.wireframes = Buffer< RenderableWireframe >();
		mScene.viewportCamera.SetPosition( Vector3f( { 1.0f, 1.0f, 1.0f } ) );
		mScene.viewportCamera.LookAt( Vector3f( { 0.0f, 0.0f, 0.0f } ) );
	}

	void SceneVisualiser::VisualiseBundle( __in const Bundler::Bundle* pBundle, __in_opt const Bundler::BundleAdditionalPayload* pAdditionalData ) 
	{
		Reset();
		
		mScene.pointClouds.Allocate( 1 );

		const uint pVertexColors = NULL;
		if ( pAdditionalData ) 
		{
			pAdditionalData->pointColors.Data();
		}

		mScene.pointClouds[ 0 ] = RenderablePointCloud( (uint)pBundle->points.Length(), pBundle->points.Data(), pVertexColors );
		mRenderer.RegisterObject( "pointCloud", &mScene.pointClouds[0] );
	}

	void SceneVisualiser::RenderScene() {
		mRenderer.Render();
	}

	void SceneVisualiser::ReshapeWindow( __in const int newWidth, __in const int newHeight ) {
		mWindowWidth = newWidth;
		mWindowHeight = newHeight;
		glViewport( 0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight );
		mScene.viewportCamera.SetPerspectiveProjection( 40.0f, (float)mWindowWidth / mWindowHeight, 0.1f, 100.0f );
	}

	void SceneVisualiser::OnMouseEvent( __in const MouseButton button, __in const MouseState state, __in const int x, __in const int y ) {
		mCameraController.OnMouseEvent( button, state, x, y );
	}

	void SceneVisualiser::OnMouseDrag( __in const int x, __in const int y ) {
		mCameraController.OnMouseDrag( x, y );
	}

	void SceneVisualiser::DrawGrid( __in const Vector3f& startPos, __in const Vector3f& endPos, __in const uint segments ) {
		mScene.grid.SetGrid( startPos, endPos, segments );
		mRenderer.RegisterObject( "grid", &mScene.grid );
	}

}