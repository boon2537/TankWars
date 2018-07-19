#pragma once
#include <vector>

typedef bool (*windows_message_handler_cb) (unsigned int msg, size_t wparam, size_t lparam); 

class Window
{
public:
	Window(const char* title, float aspectRatio);
	~Window();

	// Register a function callback to the Window.  Any time Windows processing a 
	// message, this callback should forwarded the information, along with the
	// supplied user argument. 
	void RegisterHandler(windows_message_handler_cb cb); 

	// Allow users to unregister (not needed for this Assignment, but I 
	// like having cleanup methods). 
	void UnregisterHandler(windows_message_handler_cb cb); 

	// This is safely cast-able to an HWND
	void* GetHandle() const { return m_hwnd; }

	// ** EXTRAS ** //
	// void SetTitle( char const *new_title ); 

public:
	void* m_hwnd; // intptr_t  

	// When the WindowsProcedure is called - let all listeners get first crack at the message
	// Giving us better code modularity. 
	std::vector<windows_message_handler_cb> m_handlers; 

private:
	int m_width;
	int m_height;

public:
	static Window* CreateInstance(const char* title, float aspectRatio);
	static Window* GetInstance();
	static int GetWidth();
	static int GetHeight();
};