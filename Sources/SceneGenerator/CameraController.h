#pragma once

enum class CameraEvent {
	MOVE,
	ROTATE,
	ZOOM
};

struct CameraBind {
	bool enabled;
	CameraEvent type;
};

class CameraController
{
public:
	CameraController( __in Camera& camera );

	void OnMouseEvent( __in const MouseButton button, __in const MouseState state, __in const int x, __in const int y );

	void OnMouseDrag( __in const int x, __in const int y );

protected:

	void LoadDefaultBinds();

	void MoveCamera( __in const Vector2f& delta );

	void RotateCamera( __in const Vector2f& delta );

	void ZoomCamera( __in const Vector2f& delta );

protected:

	Camera& mCamera;

	Vector2f mLastPos;

	std::map< MouseButton, CameraBind > mBinds;

};

