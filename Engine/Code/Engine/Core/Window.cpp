#include "Engine/Core/Window.hpp"
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <algorithm>    // std::find

#define GAME_WINDOW_CLASS (TEXT("Simple Window Class"))

static Window* g_Window = nullptr; 

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
// #SD1ToDo: We will move this function to a more appropriate place when we're ready
//
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	bool runDefault = true;
	Window* window = Window::GetInstance(); 
	if (window != nullptr) 
	{
		for (unsigned int i = 0; i < window->m_handlers.size(); ++i)
		{
			runDefault = window->m_handlers[i](wmMessageCode, wParam, lParam) && runDefault; 
		}
	}

	if (runDefault)
	{
		// do default windows behavior (return before this if you don't want default windows behavior for certain messages)
		return ::DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
	}
	else
	{
		return 0;
	}
}

static void RegisterWindowClass()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof( windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = GAME_WINDOW_CLASS;
	RegisterClassEx(&windowClassDescription);
}

Window::Window(const char* title, float aspectRatio)
{
	RegisterWindowClass(); //make sure this only happen once

	 // #SD1ToDo: Add support for full-screen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float) (desktopRect.right - desktopRect.left);
	float desktopHeight = (float) (desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( aspectRatio > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / aspectRatio;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * aspectRatio;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	m_height = (int) clientHeight;
	m_width = (int) clientWidth;

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, title, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[ 0 ]));
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		GAME_WINDOW_CLASS,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);

	ShowWindow((HWND) m_hwnd, SW_SHOW);
	SetForegroundWindow((HWND) m_hwnd);
	SetFocus((HWND) m_hwnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

Window::~Window()
{
	m_hwnd = nullptr;
	g_Window = nullptr;

	::DestroyWindow((HWND) m_hwnd);
	::UnregisterClass(GAME_WINDOW_CLASS, GetModuleHandle(NULL));
}

void Window::RegisterHandler(windows_message_handler_cb cb)
{
	m_handlers.push_back(cb); 
}

void Window::UnregisterHandler(windows_message_handler_cb cb)
{
	std::vector<windows_message_handler_cb>::iterator search;
	search = std::find(m_handlers.begin(), m_handlers.end(), cb);
	if (search != m_handlers.end())
	{
		m_handlers.erase(search);
	}
}

Window* Window::CreateInstance(char const *title, float aspect) 
{
	if (g_Window == nullptr) 
	{
		g_Window = new Window(title, aspect); 
	}
	return g_Window; 
}

Window* Window::GetInstance()
{
	return g_Window; 
}

int Window::GetWidth()
{
	return g_Window->m_width;
}

int Window::GetHeight()
{
	return g_Window->m_height;
}
