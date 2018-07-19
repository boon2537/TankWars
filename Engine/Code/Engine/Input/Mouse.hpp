#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"

enum MOUSEMODE
{
	MOUSEMODE_RELATIVE,
	MOUSEMODE_ABSOLUTE
};

class Mouse
{
public:
	~Mouse();
	Mouse();

	Vector2 GetMouseClientPosition() const;
	Vector2 GetCenterOfClientWindow() const;

	void SetMouseScreenPosition(const Vector2& desktopPos);
	void MouseLockToScreen(bool lock);
	void SetMouseCursorVisible(bool visible);
};
