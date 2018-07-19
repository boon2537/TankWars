#pragma once

#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"

class Camera;
class Canvas;
class TextUI;

class ProfilerView
{
public:
	~ProfilerView();
	ProfilerView();

	void StartUp();
	void Update();
	void Render();

	inline bool IsOpen() { return m_isActive; };

	void OnMouseDown(MOUSE_CODE button, const Vector2& mousePos);

	std::string ProfileEntryToString(ProfilerReportEntry* entry);
	void AppendEntryRecursivelyToString(std::string& str, ProfilerReportEntry* entry);

	std::string ProfileEntryToStringIndented(ProfilerReportEntry* entry);
	void AppendIndentedEntryRecursivelyToString(std::string& str, ProfilerReportEntry* entry, int depthInTree);

	static ProfilerView* CreateInstance();
	static ProfilerView* GetInstance();
	static void ProfilerViewToggle(Command& cmd);

public:
	bool m_treeView = true;
	bool m_sortSelf = true;
	bool m_mouseEnabled = false;

	Camera* m_uiCamera = nullptr;
	Canvas* m_canvas = nullptr;

	TextUI* m_fpsAndFrameTimeText = nullptr;
	TextUI* m_frameReportText = nullptr;
	TextUI* m_viewModeText = nullptr;

	AABB2 m_graphBounds;
	AABB2 m_graphHitBounds;

	int m_frameSelected = -1; //-1 marks not selected

private:
	bool m_isActive = false;
	float m_lastTime = 0;
};
