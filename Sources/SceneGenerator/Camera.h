#pragma once
class Camera
{
public:

	Camera();
	Camera( __in const Vector3f& position );

	void Update( __in const float dt );

	void SetPosition( __in const Vector3f& position );

	void SetTarget( __in const Vector3f& position );

	void MoveFpsStyle( __in const float forward, __in const float left, __in const float up );

	void MoveSystem( __in const float moveFactorX, __in const float moveFactorY );

	void Orbit( __in const float yawFactor, __in const float pitchFactor );

	void OrbitTarget( __in const float yawFactor, __in const float pitchFactor );

	void Zoom( __in const float zoomFactor );

	void SetPerspectiveProjection(
		__in const float fieldOfView,
		__in const float aspectRatio,
		__in const float zNear,
		__in const float zFar );

	const Matrix4x4f& GetViewMatrix() const;
	const Matrix4x4f& GetProjectionMatrix() const;

	Timer mTimer;
	void PrintAvgTimes() {
		printf( "---- Camera Time Performance (average times) ----\n" );
		printf( "\tMovement: %fms\n", mTimer.GetTimeGroup( "Movement" ).GetAverage() );
		printf( "\tRotation: %fms\n", mTimer.GetTimeGroup( "Rotation" ).GetAverage() );
		printf( "\tZoom: %fms\n", mTimer.GetTimeGroup( "Zoom" ).GetAverage() );
		printf( "\tPosition: {%ff, %ff, %ff}\n", mPosition[0], mPosition[1], mPosition[2] );
	}

protected:

	void ClampPosition( __inout Vector3f& pos );

	void RecalcViewMatrix();

protected:

	Vector3f mPosition;
	Vector3f mTarget;
	Vector3f mWorldUp;

	Matrix4x4f mViewMatrix;
	Matrix4x4f mProjectionMatrix;

	Vector3f mCameraVelocity;
	Vector3f mTargetVelocity;

};

