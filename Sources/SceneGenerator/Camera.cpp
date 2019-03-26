#include "stdafx.h"

#include "Mouse.h"
#include "AlgebraLib.h"
#include "Camera.h"

#define CAMERA_TARGET_MIN_DISTANCE 3.0f

Camera::Camera() {
	mWorldUp = Vector3f( { 0,1,0 } ); // OpenGL Y-up
	LookAt( Vector3f( { 0,0,0 } ) );
	SetPosition( Vector3f( { 1,1,1 } ) );

	mProjectionMatrix.SetIdentity();
}

Camera::Camera( __in const Vector3f& position ) {
	mWorldUp = Vector3f( { 0,1,0 } ); // OpenGL Y-up
	LookAt( Vector3f( { 0,0,0 } ) );
	SetPosition( position );

	mProjectionMatrix.SetIdentity();
}

void Camera::SetPosition( __in const Vector3f& position ) {
	mPosition = position;
	RecalcViewMatrix();
}

void Camera::MoveCamera( __in const Vector3f& moveVector ) {
	mPosition += moveVector;
	RecalcViewMatrix();
}

void Camera::MoveSystem( __in const float moveFactorX, __in const float moveFactorY ) {
#ifdef _DEBUG
	TimeGroup& stopwatch = mTimer.GetTimeGroup( "Movement" );
	stopwatch.Start();
#endif

	Vector3f forward = ( mPosition - mTarget );
	forward.Normalize();
	Vector3f left = CrossProduct( mWorldUp, forward );
	left.Normalize();
	Vector3f down = CrossProduct( left, forward );

	left *= moveFactorX;
	down *= moveFactorY;
	Vector3f moveVector = left + down;

	mPosition += moveVector;
	mTarget += moveVector;

#ifdef _DEBUG
	stopwatch.SaveElapsed();
#endif

	RecalcViewMatrix();
}

void Camera::Orbit( __in const float yawFactor, __in const float pitchFactor ) {
#ifdef _DEBUG
	TimeGroup& stopwatch = mTimer.GetTimeGroup( "Rotation" );
	stopwatch.Start();
#endif

	Vector3f forward = ( mPosition - mTarget );
	forward.Normalize();
	Vector3f left = CrossProduct( mWorldUp, forward );
	left.Normalize();
	Vector3f down = CrossProduct( left, forward );

	Matrix4x4 rotationMatrix;
	rotationMatrix.SetIdentity();
	rotationMatrix.Translate( -mTarget[0], -mTarget[1], -mTarget[2] );
	rotationMatrix.Rotate( yawFactor, down[0], down[1], down[2] );
	rotationMatrix.Rotate( pitchFactor, left[0], left[1], left[2] );
	rotationMatrix.Translate( mTarget[0], mTarget[1], mTarget[2] );

	Vector4f position( { mPosition[0], mPosition[1], mPosition[2], 1.0f } );
	position = rotationMatrix * position;
	position *= 1.0f/position[3];

#ifdef _DEBUG
	stopwatch.SaveElapsed();
#endif

	SetPosition( Vector3f( { position[0], position[1], position[2] } ) );
}

void Camera::Zoom( __in const float zoomFactor ) {
#ifdef _DEBUG
	TimeGroup& stopwatch = mTimer.GetTimeGroup( "Zoom" );
	stopwatch.Start();
#endif

	Vector3f lookDir = mTarget - mPosition;
	lookDir.Normalize();
	lookDir *= zoomFactor;

	Vector3f newPos = mPosition + lookDir;

	Vector3f targetToNewPos = newPos - mTarget;
	targetToNewPos.Normalize();
	Vector3f targetToOldPos = mPosition - mTarget;
	targetToOldPos.Normalize();
	bool targetOnSameSide = ( targetToNewPos * targetToOldPos ) > 0.0f;

	if ( ( newPos.Distance( mTarget ) < CAMERA_TARGET_MIN_DISTANCE ) || !targetOnSameSide ) {
		targetToOldPos *= CAMERA_TARGET_MIN_DISTANCE;
		newPos = mTarget + targetToOldPos;
	}

#ifdef _DEBUG
	stopwatch.SaveElapsed();
#endif

	SetPosition( newPos );
}

void Camera::LookAt( __in const Vector3f& position ) {
	mTarget = position;
	RecalcViewMatrix();
}

void Camera::SetPerspectiveProjection(
	__in const float fieldOfView,
	__in const float aspectRatio,
	__in const float zNear,
	__in const float zFar ) 
{
	mProjectionMatrix.SetPerspective( fieldOfView, aspectRatio, zNear, zFar );
}

const Matrix4x4& Camera::GetViewMatrix() const {
	return mViewMatrix;
}

const Matrix4x4& Camera::GetProjectionMatrix() const {
	return mProjectionMatrix;
}

void Camera::RecalcViewMatrix() {
	Vector3f cameraDir = (mPosition - mTarget);
	cameraDir.Normalize();
	Vector3f left = CrossProduct( mWorldUp, cameraDir );
	left.Normalize();
	Vector3f up = CrossProduct( cameraDir, left );
	
	mViewMatrix.SetIdentity();

	ShallowCopy( left.Elements(), 3, mViewMatrix[0] );
	ShallowCopy( up.Elements(), 3, mViewMatrix[1] );
	ShallowCopy( cameraDir.Elements(), 3, mViewMatrix[2] );
	
	mViewMatrix[0][3] = -left[0] * mPosition[0] - left[1] * mPosition[1] - left[2] * mPosition[2];
	mViewMatrix[1][3] = -up[0] * mPosition[0] - up[1] * mPosition[1] - up[2] * mPosition[2];
	mViewMatrix[2][3] = -cameraDir[0] * mPosition[0] - cameraDir[1] * mPosition[1] - cameraDir[2] * mPosition[2];
}
