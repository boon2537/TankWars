#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

#include "Engine/Input/Mouse.hpp"
#include "Engine/Core/Window.hpp"

Mouse::~Mouse()
{
}

Mouse::Mouse()
{
}

Vector2 Mouse::GetMouseClientPosition() const
{
	POINT desktopPos;
	::GetCursorPos(&desktopPos);

	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();
	::ScreenToClient(hwnd, &desktopPos);
	POINT client_pos = desktopPos;

	return Vector2((float) client_pos.x, (float) client_pos.y);
}

Vector2 Mouse::GetCenterOfClientWindow() const
{
	HWND hwnd = (HWND) Window::GetInstance()->GetHandle();

	RECT client_rect; // window class RECT
	::GetClientRect( hwnd, &client_rect ); 

	POINT offset; 
	offset.x = 0; 
	offset.y = 0; 
	::ClientToScreen( hwnd, &offset ); 

	client_rect.left += offset.x; 
	client_rect.right += offset.x; 
	client_rect.top += offset.y; 
	client_rect.bottom += offset.y; 

	return Vector2((client_rect.right + client_rect.left) * 0.5f, (client_rect.top + client_rect.bottom) * 0.5f);
}

void Mouse::SetMouseScreenPosition(const Vector2& desktopPos)
{
	::SetCursorPos((int) desktopPos.x, (int) desktopPos.y);
}

void Mouse::MouseLockToScreen(bool lock)
{
	if (!lock) 
	{
		::ClipCursor( nullptr ); // this unlock the mouse
	} 
	else 
	{
		HWND hwnd = (HWND) Window::GetInstance()->GetHandle();

		RECT client_rect; // window class RECT
		::GetClientRect( hwnd, &client_rect ); 

		POINT offset; 
		offset.x = 0; 
		offset.y = 0; 
		::ClientToScreen( hwnd, &offset ); 

		client_rect.left += offset.x; 
		client_rect.right += offset.x; 
		client_rect.top += offset.y; 
		client_rect.bottom += offset.y; 

		::ClipCursor( &client_rect ); 
	}
}

void Mouse::SetMouseCursorVisible(bool visible)
{
	::ShowCursor(visible); 
}
