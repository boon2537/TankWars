#include "Engine/Input/XboxController.hpp"

XboxController::XboxController()
{

}

XboxController::XboxController(unsigned int controllerID, float innerDeadZone, float outerDeadZone)
{
	m_controllerID = controllerID;
	m_LeftJoyStick.m_innerDeadZone = innerDeadZone;
	m_LeftJoyStick.m_outerDeadZone = outerDeadZone;
	m_RightJoyStick.m_innerDeadZone = innerDeadZone;
	m_RightJoyStick.m_outerDeadZone = outerDeadZone;
}

void XboxController::UpdateController()
{
	// Clear all just-changed flags
	for( int buttonCode = 0; buttonCode < XboxController::NUM_KEYS; ++ buttonCode )
	{
		m_bottonStates[buttonCode].justPressed = false;
		m_bottonStates[buttonCode].justReleased = false;
	}

	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );

	if( errorStatus == ERROR_SUCCESS )
	{
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) == XINPUT_GAMEPAD_DPAD_UP, 0);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) == XINPUT_GAMEPAD_DPAD_DOWN, 1);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) == XINPUT_GAMEPAD_DPAD_LEFT, 2);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) == XINPUT_GAMEPAD_DPAD_RIGHT, 3);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) == XINPUT_GAMEPAD_START, 4);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) == XINPUT_GAMEPAD_BACK, 5);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) == XINPUT_GAMEPAD_LEFT_THUMB, 6);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) == XINPUT_GAMEPAD_RIGHT_THUMB, 7);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == XINPUT_GAMEPAD_LEFT_SHOULDER, 8);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == XINPUT_GAMEPAD_RIGHT_SHOULDER, 9);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) == XINPUT_GAMEPAD_A, 10);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) == XINPUT_GAMEPAD_B, 11);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) == XINPUT_GAMEPAD_X, 12);
		ProcessButtonState((xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y, 13);

		m_LeftTrigger = xboxControllerState.Gamepad.bLeftTrigger / 255.f;
		m_RightTrigger = xboxControllerState.Gamepad.bRightTrigger / 255.f;

		m_LeftJoyStick.m_rawPosition.x =  xboxControllerState.Gamepad.sThumbLX;
		m_LeftJoyStick.m_rawPosition.y = xboxControllerState.Gamepad.sThumbLY;
		m_LeftJoyStick.ApplyCorrection();
		m_RightJoyStick.m_rawPosition.x =  xboxControllerState.Gamepad.sThumbRX;
		m_RightJoyStick.m_rawPosition.y = xboxControllerState.Gamepad.sThumbRY;
		m_RightJoyStick.ApplyCorrection();

		m_isActive = true;
	}
	else if ( errorStatus == ERROR_DEVICE_NOT_CONNECTED )
	{
		m_isActive = false;
		m_LeftJoyStick.Reset();
		m_RightJoyStick.Reset();
		m_LeftTrigger = 0;
		m_RightTrigger = 0;
	}
}

bool XboxController::IsButtonPressed(XBOX_BUTTON_CODE buttonCode) const
{
	if (!m_isActive)
		return false;

	return m_bottonStates[static_cast<int>(buttonCode)].isDown;
}

bool XboxController::WasButtonJustPressed(XBOX_BUTTON_CODE buttonCode) const
{
	if (!m_isActive)
		return false;

	return m_bottonStates[static_cast<int>(buttonCode)].justPressed;
}

bool XboxController::WasButtonJustReleased(XBOX_BUTTON_CODE buttonCode) const
{
	if (!m_isActive)
		return false;

	return m_bottonStates[static_cast<int>(buttonCode)].justReleased;
}

bool XboxController::IsActive() const
{
	return m_isActive;
}

const AnalogJoystick & XboxController::GetLeftJoyStick() const
{
	return m_LeftJoyStick;
}

const AnalogJoystick & XboxController::GetRightJoyStick() const
{
	return m_RightJoyStick;
}

void XboxController::ProcessButtonState(bool isBottonDown, int index)
{
	if (isBottonDown)
	{
		if (!m_bottonStates[index].isDown)
			m_bottonStates[index].justPressed = true;
		m_bottonStates[index].isDown = true;
	}
	else
	{
		if (m_bottonStates[index].isDown)
			m_bottonStates[index].justReleased = true;
		m_bottonStates[index].isDown = false;
	}
}
