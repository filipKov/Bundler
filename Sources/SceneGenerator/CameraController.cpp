#include "stdafx.h"

#include <map>

#include "GL\glew.h"
#include "GL\glut.h"

#include "AlgebraLib.h"

#include "Mouse.h"
#include "Camera.h"

#include "CameraController.h"

#define CAMERA_MOVEMENT_MULTIPIER 0.001f
#define CAMERA_ZOOM_MULTIPLIER 0.01f
#define CAMERA_ROTATION_MULTIPLIER 0.01f

CameraController::CameraController( __in Camera& camera ) : mCamera( camera ) {
	LoadDefaultBinds();
}

void CameraController::OnMouseEvent( __in const MouseButton button, __in const MouseState state, __in const int x, __in const int y ) {
	CameraBind& bind = mBinds[button];
	bind.enabled = !bind.enabled;
	mLastPos[0] = (float)x;
	mLastPos[1] = (float)y;
}

void CameraController::OnMouseDrag( __in const int x, __in const int y ) {
	Vector2f deltaPos = Vector2f( { (float)x, (float)y } ) - mLastPos;

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

void CameraController::LoadDefaultBinds() {
	mBinds[MouseButton::SECONDARY] = CameraBind { false, CameraEvent::ROTATE };
	mBinds[MouseButton::MIDDLE] = CameraBind { false, CameraEvent::ZOOM };
	mBinds[MouseButton::PRIMARY] = CameraBind { false, CameraEvent::MOVE };
}

void CameraController::MoveCamera( __in const Vector2f& delta ) {
	mCamera.MoveSystem( delta[0] * CAMERA_MOVEMENT_MULTIPIER, delta[1] * CAMERA_MOVEMENT_MULTIPIER );
}

void CameraController::RotateCamera( __in const Vector2f& delta ) {
	mCamera.Orbit( delta[0] * CAMERA_ROTATION_MULTIPLIER, delta[1] * CAMERA_ROTATION_MULTIPLIER );
}

void CameraController::ZoomCamera( __in const Vector2f& delta ) {
	float zoomFactor = delta[1] * CAMERA_ZOOM_MULTIPLIER;
	mCamera.Zoom( zoomFactor );
}