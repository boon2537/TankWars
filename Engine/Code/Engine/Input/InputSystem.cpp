#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"

void RunMessagePump() // NOTE: standalone function in InputSystem.cpp (not an InputSystem method)
{
	MSG queuedMessage;
	for( ;; )
	{
		const bool wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}

InputSystem::InputSystem()
{
	for (int controllerID = 0; controllerID < InputSystem::NUM_CONTROLLERS; ++ controllerID)
	{
		m_controllers[controllerID] = XboxController((unsigned int) controllerID, 0.16f, 0.95f);
	}

	m_positionLastFrame = m_mouse.GetMouseClientPosition();
	m_positionThisFrame = m_positionLastFrame;
}

InputSystem::~InputSystem()
{
}

void InputSystem::BeginFrame()
{
	UpdateControllers();
	UpdateKeyboard();
	UpdateMouse();
	RunMessagePump(); // Ask Windows to call our registered WinProc function with WM_KEYDOWN notifications, etc.
}

void InputSystem::EndFrame()
{

}

void InputSystem::OnKeyPressed(unsigned char keyCode)
{
	if (!m_keyStates[static_cast<int>(keyCode)].isDown)
		m_keyStates[static_cast<int>(keyCode)].justPressed = true;

	m_keyStates[static_cast<int>(keyCode)].isDown = true;
}

void InputSystem::OnKeyReleased(unsigned char keyCode)
{
	m_keyStates[static_cast<int>(keyCode)].isDown = false;
	m_keyStates[static_cast<int>(keyCode)].justReleased = true;
}

bool InputSystem::IsKeyPressed(KEY_CODE keyCode) const
{
	return m_keyStates[static_cast<int>(keyCode)].isDown;
}

bool InputSystem::WasKeyJustPressed(KEY_CODE keyCode) const
{
	return m_keyStates[static_cast<int>(keyCode)].justPressed;
}

bool InputSystem::WasKeyJustReleased(KEY_CODE keyCode) const
{
	return m_keyStates[static_cast<int>(keyCode)].justReleased;
}

void InputSystem::OnMouseDown(unsigned char keyCode)
{
	if (!m_mouseButtonStates[static_cast<int>(keyCode)].isDown)
		m_mouseButtonStates[static_cast<int>(keyCode)].justPressed = true;

	m_mouseButtonStates[static_cast<int>(keyCode)].isDown = true;
}

void InputSystem::OnMouseUp(unsigned char keyCode)
{
	m_mouseButtonStates[static_cast<int>(keyCode)].isDown = false;
	m_mouseButtonStates[static_cast<int>(keyCode)].justReleased = true;
}

bool InputSystem::IsMouseDown(MOUSE_CODE mouseCode) const
{
	return m_mouseButtonStates[static_cast<int>(mouseCode)].isDown;
}

bool InputSystem::WasMouseJustPressed(MOUSE_CODE mouseCode) const
{
	return m_mouseButtonStates[static_cast<int>(mouseCode)].justPressed;
}

bool InputSystem::WasMouseJustReleased(MOUSE_CODE mouseCode) const
{
	return m_mouseButtonStates[static_cast<int>(mouseCode)].justReleased;
}

bool InputSystem::IsShiftDown() const
{
	return IsKeyPressed(KEY_CODE::LEFT_SHIFT) || IsKeyPressed(KEY_CODE::RIGHT_SHIFT);
}

bool InputSystem::IsControlDown() const
{
	return IsKeyPressed(KEY_CODE::LEFT_CONTROLL) || IsKeyPressed(KEY_CODE::RIGHT_CONTROLL);
}

void InputSystem::SetFocused(bool focused)
{
	m_isFocused = focused;

	if (!m_isFocused)
	{
		m_positionThisFrame = Vector2::zero;
		m_positionLastFrame = Vector2::zero;
	}
	else if (m_mouseMode == MOUSEMODE::MOUSEMODE_RELATIVE)
	{
		m_positionLastFrame = m_mouse.GetCenterOfClientWindow(); 
		m_positionThisFrame = m_positionLastFrame;
		SetMousePosition(m_positionLastFrame);
	}
}

void InputSystem::UpdateMouse()
{
	if (m_isFocused)
	{
		// Absolute Mode - I get mouse position - and I can potentially lock to the screen
		m_positionLastFrame = m_positionThisFrame; 
		m_positionThisFrame = m_mouse.GetMouseClientPosition();

		// Relative mode -> I care about deltas - I reset to the center (meaning, mutually exclusive modes)
		if (m_mouseMode == MOUSEMODE::MOUSEMODE_RELATIVE) 
		{
			m_positionLastFrame = m_mouse.GetCenterOfClientWindow(); 
			SetMousePosition(m_positionLastFrame);
		}
	}

	// Clear all just-changed flags, in preparation for the next round of messages
	for (int mouseCode = 0; mouseCode < InputSystem::NUM_MOUSE_BUTTONS; ++mouseCode)
	{
		m_mouseButtonStates[mouseCode].justPressed = false;
		m_mouseButtonStates[mouseCode].justReleased = false;
	}
}

Vector2 InputSystem::GetMouseDelta()
{
	return m_positionThisFrame - m_positionLastFrame;
}

Vector2 InputSystem::GetMousePosition() const
{
	return m_positionThisFrame;
}

void InputSystem::SetMousePosition(Vector2 clientPos)
{
	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();
	POINT offset; 
	offset.x = 0; 
	offset.y = 0; 
	::ClientToScreen(hwnd, &offset); 

	m_mouse.SetMouseScreenPosition(Vector2(clientPos.x + offset.x, clientPos.y + offset.y));
}

void InputSystem::SetMouseCursorVisible(bool visible)
{
	m_mouse.SetMouseCursorVisible(visible);
}

void InputSystem::MouseLockToScreen(bool lock)
{
	m_mouse.MouseLockToScreen(lock);
}

void InputSystem::SwitchMouseMode(MOUSEMODE mode)
{
	if (mode != m_mouseMode)
	{
		m_mouseMode = mode;
		if (m_mouseMode == MOUSEMODE::MOUSEMODE_RELATIVE)
		{
			m_positionLastFrame = m_mouse.GetCenterOfClientWindow(); 
			SetMousePosition(m_positionLastFrame);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void InputSystem::UpdateKeyboard()
{
	// Clear all just-changed flags, in preparation for the next round of WM_KEYDOWN, etc. messages
	for( int keyCode = 0; keyCode < InputSystem::NUM_KEYS; ++ keyCode )
	{
		m_keyStates[keyCode].justPressed = false;
		m_keyStates[keyCode].justReleased = false;
	}
}

void InputSystem::UpdateControllers()
{
	for (int controllerID = 0; controllerID < InputSystem::NUM_CONTROLLERS; ++ controllerID)
	{
		m_controllers[controllerID].UpdateController();
	}
}
