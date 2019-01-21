#pragma once

class Camera
{
public:

	Camera();
	Camera( __in const Vector3f& position );

	void SetPosition( __in const Vector3f& position );

	void MoveCamera( __in const Vector3f& moveVector );

	void MoveSystem( __in const float moveFactorX, __in const float moveFactorY );

	void Orbit( __in const float yawFactor, __in const float pitchFactor );

	void Zoom( __in const float zoomFactor );

	void LookAt( __in const Vector3f& position );

	void SetPerspectiveProjection(
		__in const float fieldOfView,
		__in const float aspectRatio,
		__in const float zNear,
		__in const float zFar );

	const Matrix4x4& GetViewMatrix() const;
	const Matrix4x4& GetProjectionMatrix() const;

#ifdef _DEBUG
	Timer mTimer;
	void PrintAvgTimes() {
		printf( "---- Camera Time Performance (average times) ----\n" );
		printf( "\tMovement: %fms\n", mTimer.GetTimeGroup( "Movement" ).GetAverage() );
		printf( "\tRotation: %fms\n", mTimer.GetTimeGroup( "Rotation" ).GetAverage() );
		printf( "\tZoom: %fms\n", mTimer.GetTimeGroup( "Zoom" ).GetAverage() );
	}
#endif

protected:

	void RecalcViewMatrix();

protected:

	Vector3f mPosition;
	Vector3f mTarget;
	Vector3f mWorldUp;

	Matrix4x4 mViewMatrix;
	Matrix4x4 mProjectionMatrix;



};

