#pragma once

class KeyButtonState
{
public:
	~KeyButtonState() {}
	KeyButtonState() { isDown = false; justPressed = false; justReleased = false; }

	bool isDown;
	bool justPressed;
	bool justReleased;
};