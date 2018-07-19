#pragma once

#include "Game/Game.hpp"

class App
{
public:
	App();
	~App();
	bool IsQuitting();
	void Quit();
	void Update();
	void Render();
	void BeginFrame();
	void EndFrame();

	void OnQuitRequested();

private:
	bool m_quitting;
};
