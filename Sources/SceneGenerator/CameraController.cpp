#include "stdafx.h"

#include "Mouse.h"
#include "KeyMacros.h"
#include "Camera.h"

#include "CameraController.h"

// #define CAMERA_MOVEMENT_MULTIPIER 0.0075f
// #define CAMERA_ZOOM_MULTIPLIER 0.025f
// #define CAMERA_ROTATION_MULTIPLIER 0.05f
// #define CAMERA_FPS_MOVEMENT_MULTIPLIER 0.5f

#define CAMERA_MOVEMENT_MULTIPIER 0.01f
#define CAMERA_ZOOM_MULTIPLIER 0.05f
#define CAMERA_ROTATION_MULTIPLIER 0.05f
#define CAMERA_FPS_MOVEMENT_MULTIPLIER 1.0f

CameraController::CameraController() {
	m_pCamera = nullptr;
	LoadDefaultBinds();
	mLastPos = Vector<float, 2>();
}

void CameraController::SetCamera( __in Camera* pCamera ) {
	m_pCamera = pCamera;
}


void CameraController::OnMouseEvent( __in const MouseButton button, __in const MouseState state, __in const int x, __in const int y ) {
	CameraBind& bind = mBinds[button];
	bind.enabled = state == MouseState::MOUSE_DOWN ? true : false;
	mLastPos[0] = (float)x;
	mLastPos[1] = (float)y;
}

void CameraController::OnMouseDrag( __in const int x, __in const int y ) {
	Vector< float, 2 > deltaPos = Vector<float, 2>( { (float)x, (float)y } ) - mLastPos;

	for ( auto bindPair = mBinds.begin(); bindPair != mBinds.end(); bindPair++ ) {
		CameraBind& bind = bindPair->second;
		if ( bind.enabled ) {
			switch ( bind.type ) {
				case CameraEvent::MOVE:
					MoveCamera( deltaPos );
					break;
				case CameraEvent::ROTATE:
					RotateCamera( deltaPos );
					break;
				case CameraEvent::ZOOM:
					ZoomCamera( deltaPos );
					break;
			}
		}
	}

}

void CameraController::OnKeyDown( __in const uint key, __in const int x, __in const int y ) {
	UNREFERENCED_PARAMETER( x );
	UNREFERENCED_PARAMETER( y );

	float forward	= ( key == KEY_W ? 1.0f : ( key == KEY_S ? -1.0f : 0.0f ) ) * CAMERA_FPS_MOVEMENT_MULTIPLIER;
	float left		= ( key == KEY_A ? 1.0f : ( key == KEY_D ? -1.0f : 0.0f ) ) * CAMERA_FPS_MOVEMENT_MULTIPLIER;
	float up		= ( key == KEY_Q ? 1.0f : ( key == KEY_E ? -1.0f : 0.0f ) ) * CAMERA_FPS_MOVEMENT_MULTIPLIER;
	m_pCamera->MoveFpsStyle( forward, left, up );
}

void CameraController::OnKeyUp( __in const uint key, __in const int x, __in const int y ) {
	// TODO
	UNREFERENCED_PARAMETER( key );
	UNREFERENCED_PARAMETER( x );
	UNREFERENCED_PARAMETER( y );
}

void CameraController::SetCtrlActive() {
	mCtrlActive = true;
}

void CameraController::SetCrtlInactive() {
	mCtrlActive = false;
}

void CameraController::Update( __in const float t ) {
	m_pCamera->Update( t );
}

void CameraController::Clear() {
	mLastPos = Vector<float, 2>();
	LoadDefaultBinds();
	m_pCamera = nullptr;
}

void CameraController::LoadDefaultBinds() {
	mBinds[MouseButton::PRIMARY] = CameraBind { false, CameraEvent::ROTATE };
	mBinds[MouseButton::MIDDLE] = CameraBind { false, CameraEvent::ZOOM };
	mBinds[MouseButton::SECONDARY] = CameraBind { false, CameraEvent::MOVE };
}

void CameraController::MoveCamera( __in const Vector< float, 2 >& delta ) {
	m_pCamera->MoveSystem( delta[0] * CAMERA_MOVEMENT_MULTIPIER, delta[1] * CAMERA_MOVEMENT_MULTIPIER );
}

void CameraController::RotateCamera( __in const Vector< float, 2 >& delta ) {
	if ( mCtrlActive ) {
		m_pCamera->Orbit( delta[0] * CAMERA_ROTATION_MULTIPLIER, delta[1] * CAMERA_ROTATION_MULTIPLIER );
	} else {
		// TOOD
		m_pCamera->OrbitTarget( delta[0] * CAMERA_ROTATION_MULTIPLIER, delta[1] * CAMERA_ROTATION_MULTIPLIER );
	}
}

void CameraController::ZoomCamera( __in const Vector< float, 2 >& delta ) {
	float zoomFactor = delta[1] * CAMERA_ZOOM_MULTIPLIER;
	m_pCamera->Zoom( zoomFactor );
}