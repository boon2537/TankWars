#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>

#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Profiler/ProfilerView.hpp"
#include "Engine/Profiler/Profiler.hpp"

#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to GameCommon.hpp or elsewhere
// 
constexpr float CLIENT_ASPECT = 16.f / 9.f; // We are requesting a 1:1 aspect (square) window area

const char* APP_NAME = "Win32 OpenGL Test App";	// ...becomes ???

App* g_theApp;

void QuitGameCommand(Command& cmd) 
{
	UNUSED(cmd);

	g_theApp->Quit();
}

bool AppMessageHandler(unsigned int wmMessageCode, size_t wParam, size_t lParam)
{
	UNUSED(lParam);

	unsigned char keyCode = (unsigned char) wParam; 
	switch (wmMessageCode) 
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		g_theApp->OnQuitRequested();
		break;

	case WM_KEYDOWN:
		// to disable input while DevConsole is open
		if (!DevConsoleIsOpen())
		{
			g_theInput->OnKeyPressed(keyCode);
		}
		break;

	case WM_KEYUP:
		if (!DevConsoleIsOpen())
		{
			g_theInput->OnKeyReleased(keyCode);
		}
		break;

	case WM_LBUTTONDOWN: 
		g_theInput->OnMouseDown(0);
		break;
	case WM_LBUTTONUP:
		g_theInput->OnMouseUp(0);
		break;

	case WM_RBUTTONDOWN:
		g_theInput->OnMouseDown(1);
		break;
	case WM_RBUTTONUP:
		g_theInput->OnMouseUp(1);
		break;

	case WM_ACTIVATE:
		bool is_active = (WA_INACTIVE != LOWORD(wParam));
		g_theInput->SetFocused(is_active);
		g_theInput->MouseLockToScreen(is_active);
		break;
	};

	return true;
}

void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle, float clientAspect)
{
	UNUSED(applicationInstanceHandle);

	Window* window = Window::CreateInstance(APP_NAME, clientAspect); 
	window->RegisterHandler(AppMessageHandler); 
}

//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	ProfilerMarkFrame();

	g_theApp->BeginFrame();
	g_theApp->Update();
	g_theApp->Render();
	g_theApp->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	CreateOpenGLWindow( applicationInstanceHandle, CLIENT_ASPECT );
	g_theApp = new App();

	CommandRegister("quit", QuitGameCommand, "Quits the application.");

	g_theRenderer->RenderStartup((HWND) Window::GetInstance()->GetHandle());
	g_theRenderer->PostStartup();
	g_theRenderer->SetProjectionMatrix(Matrix44::MakeOrtho(Vector2(0, 0), Vector2((float) Window::GetWidth(), (float) Window::GetHeight()), -1, 1));

	g_theGame->Initialize();
	DevConsole::GetInstance()->StartUp();

	if (ProfilerView::GetInstance() != nullptr)
		ProfilerView::GetInstance()->StartUp();

	CommandRunScriptFile("Data/startup.script");
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	g_theRenderer->GLShutdown();

	// Destroy the global App instance
	delete g_theApp;
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int ){
	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() )
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}
