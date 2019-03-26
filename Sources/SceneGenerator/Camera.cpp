#include "stdafx.h"

#include "Mouse.h"

#include "Camera.h"

#define CAMERA_TARGET_MIN_DISTANCE 3.0f
#define EPS 0.000001f

Camera::Camera() {
	mWorldUp = Vector3f( { 0,1,0 } ); // OpenGL Y-up
	SetTarget( Vector3f( { 0,0,0 } ) );
	SetPosition( Vector3f( { 1,1,1 } ) );

	mProjectionMatrix.SetIdentity();
}

Camera::Camera( __in const Vector3f& position ) {
	mWorldUp = Vector3f( { 0,1,0 } ); // OpenGL Y-up
	SetTarget( Vector3f( { 0,0,0 } ) );
	SetPosition( position );

	mProjectionMatrix.SetIdentity();
}

void Camera::Update( __in const float dt ) {
	mTarget += mTargetVelocity * ( dt );

	Vector3f newPos = mPosition + ( mCameraVelocity * dt );
	ClampPosition( newPos );
	mPosition = newPos;

	RecalcViewMatrix();
	mCameraVelocity *= 0.9f;
	mTargetVelocity *= 0.9f;
}

void Camera::SetPosition( __in const Vector3f& position ) {
	mPosition = position;
	mCameraVelocity *= 0;
	Update( 0.0f );
}

void Camera::SetTarget( __in const Vector3f& position ) {
	mTarget = position;
	mTargetVelocity *= 0;
	Update( 0.0f );
}

void Camera::MoveFpsStyle( __in const float forward, __in const float left, __in const float up ) {
	Vector3f forwardDir = mTarget - mPosition;
	forwardDir.Normalize();

	Vector3f leftDir = CrossProduct( mWorldUp, forwardDir );
	leftDir.Normalize();

	Vector3f upDir = CrossProduct( forwardDir, leftDir );
	upDir.Normalize();

	Vector3f moveVector = ( forwardDir * forward ) + ( leftDir * left ) + ( upDir * up );
	mCameraVelocity += moveVector;
	mTargetVelocity += moveVector;
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
	
	mCameraVelocity += moveVector;
	mTargetVelocity += moveVector;

#ifdef _DEBUG
	stopwatch.SaveElapsed();
#endif
}

void Camera::Orbit( __in const float yawFactor, __in const float pitchFactor ) {
#ifdef _DEBUG
	TimeGroup& stopwatch = mTimer.GetTimeGroup( "Rotation" );
	stopwatch.Start();
#endif

	Vector3f forward = ( mTarget - mPosition );
	forward.Normalize();
	Vector3f left = CrossProduct( mWorldUp, forward );
	left.Normalize();
	Vector3f down = CrossProduct( left, forward );

	float rotationMatrix[16];
	M44Identity( rotationMatrix );
	M44Translate( ( -mTarget ).Elements( ), rotationMatrix );
	M44Rotate( yawFactor, down.Elements( ), rotationMatrix );
	M44Rotate( pitchFactor, left.Elements( ), rotationMatrix );
	M44Translate( mTarget.Elements( ), rotationMatrix );

	
	float pos4[4] = { mPosition[0], mPosition[1], mPosition[2], 1.0f };
	Vector4f position;
	
	M44MulV4( rotationMatrix, pos4, position.Elements( ) );
	position *= 1.0f / position[3];

	Vector3f moveVector = Vector3f( { position[0], position[1], position[2] } ) - mPosition;
	mCameraVelocity += moveVector;

#ifdef _DEBUG
	stopwatch.SaveElapsed();
#endif
}

void Camera::OrbitTarget( __in const float yawFactor, __in const float pitchFactor ) {
	Vector3f forward = ( mPosition - mTarget );
	forward.Normalize();
	Vector3f left = CrossProduct( mWorldUp, forward );
	left.Normalize();
	Vector3f down = CrossProduct( left, forward );

	float rotationMatrix[16];
	M44Identity( rotationMatrix );
	M44Translate( ( -mPosition ).Elements( ), rotationMatrix );
	M44Rotate( yawFactor, down.Elements( ), rotationMatrix );
	M44Rotate( pitchFactor, left.Elements( ), rotationMatrix );
	M44Translate( mPosition.Elements( ), rotationMatrix );

	float target4[4] = { mTarget[0], mTarget[1], mTarget[2], 1.0f };
	Vector4f newTarget;

	M44MulV4( rotationMatrix, target4, newTarget.Elements( ) );
	newTarget *= 1.0f / newTarget[3];

	Vector3f moveVector = Vector3f( { newTarget[0], newTarget[1], newTarget[2] } ) - mTarget;
	mTargetVelocity += moveVector;
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

	Vector3f moveVector = newPos - mPosition;
	mCameraVelocity += moveVector;

#ifdef _DEBUG
	stopwatch.SaveElapsed();
#endif
}

void Camera::SetPerspectiveProjection(
	__in const float fieldOfView,
	__in const float aspectRatio,
	__in const float zNear,
	__in const float zFar ) 
{
	M44SetPerspective( fieldOfView, aspectRatio, zNear, zFar, mProjectionMatrix.Elements( ) );
}

const Matrix4x4f& Camera::GetViewMatrix() const {
	return mViewMatrix;
}

const Matrix4x4f& Camera::GetProjectionMatrix() const {
	return mProjectionMatrix;
}

void Camera::ClampPosition( __inout Vector3f& pos ) {
	Vector3f targetToNewPos = pos - mTarget;
	targetToNewPos.Normalize();
	Vector3f targetToOldPos = mPosition - mTarget;
	targetToOldPos.Normalize();
	bool targetOnSameSide = ( targetToNewPos * targetToOldPos ) > 0.0f;
	float distanceToTarget = pos.Distance( mTarget );

	if ( !targetOnSameSide ) {
		pos = targetToOldPos * CAMERA_TARGET_MIN_DISTANCE;
	} else if ( distanceToTarget < CAMERA_TARGET_MIN_DISTANCE ) {
		pos += targetToNewPos * ( CAMERA_TARGET_MIN_DISTANCE - distanceToTarget );
	}
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