#pragma once

#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/AnalogJoyStick.hpp"


enum class XBOX_BUTTON_CODE
{
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	START,
	BACK,
	LEFT_THUMB,
	RIGHT_THUMB,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	A,
	B,
	X,
	Y
};


class XboxController
{
public:
	~XboxController() {}
	XboxController();
	XboxController(unsigned int controllerID, float innerDeadZone, float outerDeadZone);

public:
	static const int NUM_KEYS = 14;
	unsigned int m_controllerID;
	float m_LeftTrigger;
	float m_RightTrigger;

	void UpdateController();
	bool IsButtonPressed(const XBOX_BUTTON_CODE keyCode) const;
	bool WasButtonJustPressed(XBOX_BUTTON_CODE keyCode) const;
	bool WasButtonJustReleased(XBOX_BUTTON_CODE keyCode) const;
	bool IsActive() const;
	const AnalogJoystick& GetLeftJoyStick() const;
	const AnalogJoystick& GetRightJoyStick() const;

protected:
	void ProcessButtonState(bool isBottonDown, int index);

protected:
	KeyButtonState m_bottonStates[NUM_KEYS];
	bool m_isActive = false;
	AnalogJoystick m_LeftJoyStick;
	AnalogJoystick m_RightJoyStick;
};