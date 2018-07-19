#pragma once

#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/Mouse.hpp"
#include "Engine/Math/Vector2.hpp"

enum class KEY_CODE
{
	NONE = 0, //	Not assigned (never returned as the result of a keystroke).
	BACKSPACE = 8, //	The backspace key.
	DEL = 46, //	The forward delete key.
	TAB = 9, //	The tab key.
	ESC = 27, //	Escape key.
	SPACE = 32, //	Space key.
	KEYPAD0 = 96, //	Numeric keypad 0.
	KEYPAD1 = 97, //	Numeric keypad 1.
	KEYPAD2 = 98, //	Numeric keypad 2.
	KEYPAD3 = 99, //	Numeric keypad 3.
	KEYPAD4 = 100, //	Numeric keypad 4.
	KEYPAD5 = 101, //	Numeric keypad 5.
	KEYPAD6 = 102, //	Numeric keypad 6.
	KEYPAD7 = 103, //	Numeric keypad 7.
	KEYPAD8 = 104, //	Numeric keypad 8.
	KEYPAD9 = 105, //	Numeric keypad 9.
	KEYPAD_PERIOD = 110, //	Numeric keypad '.'.
	KEYPAD_DIVIDE = 111, //	Numeric keypad '/'.
	KEYPAD_MULTIPLY = 106, //	Numeric keypad '*'.
	KEYPAD_MINUS = 109, //	Numeric keypad '-'.
	KEYPAD_PLUS = 107, //	Numeric keypad '+'.
	KEYPAD_ENTER = 13, //	Numeric keypad enter.
	UP_ARROW = 38, //	Up arrow key.
	DOWN_ARROW = 40, //	Down arrow key.
	RIGHT_ARROW = 39, //	Right arrow key.
	LEFT_ARROW = 37, //	Left arrow key.
	INSERT = 45, //	Insert key key.
	HOME = 36, //	Home key.
	END = 35, //	End key.
	PAGE_UP = 33, //	Page up.
	PAGE_DOWN = 34, //	Page down.
	RETURN = 13, // Return (enter) key
	F1 = 112, //	F1 function key.
	F2 = 113, //	F2 function key.
	F3 = 114, //	F3 function key.
	F4 = 115, //	F4 function key.
	F5 = 116, //	F5 function key.
	F6 = 117, //	F6 function key.
	F7 = 118, //	F7 function key.
	F8 = 119, //	F8 function key.
	F9 = 120, //	F9 function key.
	F10 = 121, //	F10 function key.
	F11 = 122, //	F11 function key.
	F12 = 123, //	F12 function key.
	ALPHA0 = 48, //	The '0' key on the top of the alphanumeric keyboard.
	ALPHA1 = 49, //	The '1' key on the top of the alphanumeric keyboard.
	ALPHA2 = 50, //	The '2' key on the top of the alphanumeric keyboard.
	ALPHA3 = 51, //	The '3' key on the top of the alphanumeric keyboard.
	ALPHA4 = 52, //	The '4' key on the top of the alphanumeric keyboard.
	ALPHA5 = 53, //	The '5' key on the top of the alphanumeric keyboard.
	ALPHA6 = 54, //	The '6' key on the top of the alphanumeric keyboard.
	ALPHA7 = 55, //	The '7' key on the top of the alphanumeric keyboard.
	ALPHA8 = 56, //	The '8' key on the top of the alphanumeric keyboard.
	ALPHA9 = 57, //	The '9' key on the top of the alphanumeric keyboard.
	QUOTE = 222, //	Quote key '.
	COMMA = 188, //	Comma ',' key.
	MINUS = 189, //	Minus '-' key.
	PERIOD = 190, //	Period '.' key.
	SLASH = 191, //	Slash '/' key.
	SEMICOLON = 59, //	Semicolon ';' key.
	LESS = 188, //	Less than '<' key.
	EQUALS = 61, //	Equals '=' key.
	GREATER = 190, //	Greater than '>' key.
	LEFT_BRACKET = 219, //	Left square bracket key '['.
	BACKSLASH = 220, //	Backslash key '\'.
	RIGHT_BRACKET = 221, //	Right square bracket key ']'.
	BACK_QUOTE = 192, //	Back quote key '`'.
	A = 65, //	'a' key.
	B = 66, //	'b' key.
	C = 67, //	'c' key.
	D = 68, //	'd' key.
	E = 69, //	'e' key.
	F = 70, //	'f' key.
	G = 71, //	'g' key.
	H = 72, //	'h' key.
	I = 73, //	'i' key.
	J = 74, //	'j' key.
	K = 75, //	'k' key.
	L = 76, //	'l' key.
	M = 77, //	'm' key.
	N = 78, //	'n' key.
	O = 79, //	'o' key.
	P = 80, //	'p' key.
	Q = 81, //	'q' key.
	R = 82, //	'r' key.
	S = 83, //	's' key.
	T = 84, //	't' key.
	U = 85, //	'u' key.
	V = 86, //	'v' key.
	W = 87, //	'w' key.
	X = 88, //	'x' key.
	Y = 89, //	'y' key.
	Z = 90, //	'z' key.
	NUM_LOCK = 144, //	Numlock key.
	CAPS_LOCK = 20, //	Capslock key.
	SCROLL_LOCK = 145, //	Scroll lock key.
	RIGHT_SHIFT = 161, //	Right shift key.
	LEFT_SHIFT = 16, //	Left shift key.
	RIGHT_CONTROLL = 163, //	Right Control key.
	LEFT_CONTROLL = 17, //	Left Control key.
	RIGHT_ALT = 165, //	Right Alt key.
	LEFT_ALT= 164, //	Left Alt key.
	RIGHT_WINDOWS = 92, //	Right Windows key.
	LEFT_WINDOWS = 91, //	Left Windows key.
};

enum class MOUSE_CODE
{
	BUTTON_LEFT,	//The Left (or primary) mouse button.
	BUTTON_RIGHT,	//Right mouse button (or secondary mouse button).

	//NO SUPPORT YET
// 	BUTTON_MIDDLE,	//Middle mouse button (or third button).
// 	Mouse3,			//Additional (fourth) mouse button.
// 	Mouse4,			//Additional (fifth) mouse button.
// 	Mouse5,			//Additional (or sixth) mouse button.
// 	Mouse6,			//Additional (or seventh) mouse button.
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	void BeginFrame();
	void EndFrame();

	void OnKeyPressed(unsigned char keyCode);
	void OnKeyReleased(unsigned char keyCode);
	bool IsKeyPressed(KEY_CODE keyCode) const;
	bool WasKeyJustPressed(KEY_CODE keyCode) const;
	bool WasKeyJustReleased(KEY_CODE keyCode) const;

	void OnMouseDown(unsigned char keyCode);
	void OnMouseUp(unsigned char keyCode);
	bool IsMouseDown(MOUSE_CODE mouseCode) const;
	bool WasMouseJustPressed(MOUSE_CODE mouseCode) const;
	bool WasMouseJustReleased(MOUSE_CODE mouseCode) const;

	bool IsShiftDown() const;
	bool IsControlDown() const;

	XboxController& GetController(int controllerID) { return m_controllers[ controllerID ]; }

	void SetFocused(bool focused);
	void UpdateMouse();
	Vector2 GetMouseDelta();
	Vector2 GetMousePosition() const;
	void SetMousePosition(Vector2 clientPos);
	void SetMouseCursorVisible(bool visible);
	void MouseLockToScreen(bool lock);
	void SwitchMouseMode(MOUSEMODE mode);
	inline MOUSEMODE GetMouseMode() const { return m_mouseMode; }

	//only DevConsole should call this directly...
	void UpdateKeyboard();

public:
	static const int NUM_KEYS = 256;
	static const int NUM_CONTROLLERS = 4;
	static const int NUM_MOUSE_BUTTONS = 7;

	float m_wheelDelta = 0;

protected:
	void UpdateControllers();

protected:
	KeyButtonState m_keyStates[NUM_KEYS];
	KeyButtonState m_mouseButtonStates[NUM_MOUSE_BUTTONS];
	XboxController m_controllers[NUM_CONTROLLERS];

	Mouse m_mouse;
	MOUSEMODE m_mouseMode = MOUSEMODE::MOUSEMODE_ABSOLUTE;
	Vector2 m_positionLastFrame;
	Vector2 m_positionThisFrame;

	bool m_isFocused = true;
};

void RunMessagePump();
