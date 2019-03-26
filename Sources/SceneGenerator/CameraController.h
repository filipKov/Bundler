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
	CameraController();

	void SetCamera( __in Camera* pCamera );

	void OnMouseEvent( __in const MouseButton button, __in const MouseState state, __in const int x, __in const int y );

	void OnMouseDrag( __in const int x, __in const int y );

	void OnKeyDown( __in const uint key, __in const int x, __in const int y );

	void OnKeyUp( __in const uint key, __in const int x, __in const int y );

	void SetCtrlActive();

	void SetCrtlInactive();

	void Update( __in const float t );

	void Clear();

protected:

	void LoadDefaultBinds();

	void MoveCamera( __in const Vector< float, 2 >& delta );

	void RotateCamera( __in const Vector< float, 2 >& delta );

	void ZoomCamera( __in const Vector< float, 2 >& delta );

protected:

	Camera* m_pCamera;

	Vector< float, 2 > mLastPos;

	// std::mutex mUpdateLock;

	std::map< MouseButton, CameraBind > mBinds;

	bool mCtrlActive;

};

