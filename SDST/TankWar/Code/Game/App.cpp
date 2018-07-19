#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerView.hpp"

static MOUSEMODE PREV_MOUSE_MODE;

App::App()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("Data/GameConfig.xml");
	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*doc.RootElement());

	g_theRenderer = Renderer::CreateInstance();
	g_theInput = new InputSystem();
	g_audio = AudioSystem::CreateInstance();
	g_theGame = new Game();
	DevConsole::CreateInstance();
	ProfilerView::CreateInstance();
	ClockSystemStartup();
	DebugRender::CreateInstance();
	ProfilingSystemStartup();

	m_quitting = false;	
}

App::~App()
{
	ProfilingSystemShutdown();
	delete g_theGame;
	delete g_audio;
	delete g_theInput;
	delete g_theRenderer;
}

bool App::IsQuitting()
{
	return m_quitting;
}

void App::Quit()
{
	m_quitting = true;
}

void App::Update()
{
	PROFILE_SCOPE_FUNCTION(); 

	DevConsole* console = DevConsole::GetInstance();
	ProfilerView* profilerView = ProfilerView::GetInstance();

	if (g_theInput->WasKeyJustPressed(KEY_CODE::ESC) && !console->IsOpen() 
		|| (g_theInput->WasKeyJustPressed(KEY_CODE::RETURN) && g_theGame->m_currentState == GAME_STATE::ATTRACT && g_theGame->tempMenuIndex == 1)) //hacky remove later
	{
		Quit();
	}
	if (g_theInput->WasKeyJustPressed(KEY_CODE::F8))
	{
		g_theRenderer->TakeScreenShot();
	}
	if (g_theInput->WasKeyJustPressed(DevConsole::ACTIVATE_KEY))
	{
		if (!console->IsOpen())
		{
			g_theInput->SetFocused(false);
			g_theInput->MouseLockToScreen(false);
			g_theInput->SetMouseCursorVisible(true);
			console->Open();
		}
	}
	if (console->IsOpen())
	{
		console->Update();

		//just closed
		if (!console->IsOpen())
		{
			g_theInput->SetFocused(true);
			g_theInput->MouseLockToScreen(true);
			g_theInput->SetMouseCursorVisible(false);
		}
	}

	//Update ProfilerView
	if (profilerView != nullptr && profilerView->IsOpen())
	{
		//catch inputs
		//Toggle flat/tree view
		if (g_theInput->WasKeyJustPressed(KEY_CODE::V))
		{
			profilerView->m_treeView = !profilerView->m_treeView;
		}
		//Toggle sort for flat view (total/self)
		if (g_theInput->WasKeyJustPressed(KEY_CODE::L))
		{
			profilerView->m_sortSelf = !profilerView->m_sortSelf;
		}
		//Toggle mouse
		if (g_theInput->WasKeyJustPressed(KEY_CODE::M))
		{
			profilerView->m_mouseEnabled = !profilerView->m_mouseEnabled;

			if (profilerView->m_mouseEnabled)
			{
				PREV_MOUSE_MODE = g_theInput->GetMouseMode();

				g_theInput->SetMouseCursorVisible(true);

				if (PREV_MOUSE_MODE == MOUSEMODE::MOUSEMODE_RELATIVE)
					g_theInput->SwitchMouseMode(MOUSEMODE::MOUSEMODE_ABSOLUTE);
			}
			else
			{
				g_theInput->SetMouseCursorVisible(false);

				if (PREV_MOUSE_MODE == MOUSEMODE::MOUSEMODE_RELATIVE)
					g_theInput->SwitchMouseMode(MOUSEMODE::MOUSEMODE_RELATIVE);
			}
		}
		if (g_theInput->WasMouseJustPressed(MOUSE_CODE::BUTTON_LEFT))
		{
			profilerView->OnMouseDown(MOUSE_CODE::BUTTON_LEFT, g_theInput->GetMousePosition());
		}
		if (g_theInput->WasMouseJustPressed(MOUSE_CODE::BUTTON_RIGHT))
		{
			profilerView->OnMouseDown(MOUSE_CODE::BUTTON_RIGHT, g_theInput->GetMousePosition());
		}

		profilerView->Update();
	}

	g_theGame->Update();
}

void App::Render()
{
	PROFILE_SCOPE_FUNCTION(); 

	g_theGame->Render();

	DebugRender::GetInstance()->DebugRenderUpdateAndRender();

	ProfilerView* profilerView = ProfilerView::GetInstance();
	if (profilerView != nullptr && profilerView->IsOpen())
	{
		profilerView->Render();
	}

	DevConsole* console = DevConsole::GetInstance();
	if (console->IsOpen())
	{
		console->Render(g_theRenderer);
	}
}

void App::BeginFrame()
{
	ClockSystemBeginFrame();
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_audio->BeginFrame();
}

void App::EndFrame()
{
	g_audio->EndFrame();
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

void App::OnQuitRequested()
{
	Quit();
}
