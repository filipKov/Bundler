#pragma once

enum class MouseState {
	MOUSE_DOWN = GLUT_DOWN,
	MOUSE_UP = GLUT_UP
};

enum class MouseButton {
	PRIMARY = GLUT_LEFT_BUTTON,
	MIDDLE = GLUT_MIDDLE_BUTTON,
	SECONDARY = GLUT_RIGHT_BUTTON
};