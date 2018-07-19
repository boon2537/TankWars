#include "Engine/Profiler/ProfilerView.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/Window.hpp"

#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/TextUI.hpp"
#include "Engine/UI/ImageUI.hpp"
#include <math.h> 

static ProfilerView* g_profilerView = nullptr;

constexpr uint FUNCTION_SPACING = 40;
constexpr uint SPACE_PER_INDENT = 2;

constexpr uint GRAPH_WIDTH = 800;
constexpr uint GRAPH_HEIGHT = 200;
static Vector2 GRAPH_BOT_LEFT = Vector2(700, 750);

ProfilerView::~ProfilerView()
{
}

ProfilerView::ProfilerView()
{
	m_graphBounds = AABB2(GRAPH_BOT_LEFT, Vector2(GRAPH_BOT_LEFT.x + GRAPH_WIDTH, GRAPH_BOT_LEFT.y + GRAPH_HEIGHT));
	m_graphHitBounds = AABB2(GRAPH_BOT_LEFT.x, Window::GetHeight() - GRAPH_BOT_LEFT.y - GRAPH_HEIGHT, GRAPH_BOT_LEFT.x + GRAPH_WIDTH, Window::GetHeight() - GRAPH_BOT_LEFT.y);

	CommandRegister("profiler", ProfilerView::ProfilerViewToggle, "Toggle ProfilerView");
}

void ProfilerView::StartUp()
{
	Renderer* r = Renderer::GetInstance();

	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget(r->GetDefaultColorTarget());
	m_uiCamera->SetDepthStencilTarget(r->GetDefaultDepthTarget());

	m_canvas = new Canvas();
	m_canvas->SetUICamera(m_uiCamera);

	m_canvas->m_canvasGroups.push_back(new CanvasGroup(m_canvas)); //group 0 background
	m_canvas->m_canvasGroups.push_back(new CanvasGroup(m_canvas)); //group 1 texts
	ImageUI* image = new ImageUI(m_canvas);
	image->SetBounds(AABB2::ZERO_TO_ONE);
	image->m_tint = Rgba(100, 150, 100, 200);
	m_canvas->m_canvasGroups[0]->m_elements.push_back(image);

	TextUI* t = new TextUI(m_canvas);
	t->SetBounds(AABB2(0.05f, 0.1f, 1.f, 0.7f));
	t->m_height = 14.f;
	m_frameReportText = t;
	m_canvas->m_canvasGroups[1]->m_elements.push_back(t);

	t = new TextUI(m_canvas);
	t->SetBounds(AABB2(0.05f, 0.75f, 1.f, 0.8f));
	t->m_height = 18.f;
	m_viewModeText = t;
	m_canvas->m_canvasGroups[1]->m_elements.push_back(t);

	t = new TextUI(m_canvas);
	t->SetBounds(AABB2(0.05f, 0.85f, 1.f, 0.9f));
	t->m_height = 18.f;
	m_fpsAndFrameTimeText = t;
	m_canvas->m_canvasGroups[1]->m_elements.push_back(t);
}

void ProfilerView::Update()
{
	Profiler* profiler = Profiler::GetInstance();
	ProfilerReport* report = ProfilerReport::GetInstance();

	int frameToUse = 0;
	if (m_frameSelected > 0)
		frameToUse = m_frameSelected;

	if (profiler->ProfileGetPreviousFrame(frameToUse) != nullptr)
	{
		if (m_treeView)
		{
			report->GenerateReportTreeFromFrame(profiler->ProfileGetPreviousFrame(frameToUse));
			m_frameReportText->SetText(ProfileEntryToStringIndented(report->m_root));
		}
		else
		{
			report->GenerateReportFlatFromFrame(profiler->ProfileGetPreviousFrame(frameToUse));

			//a report is SortSelf by default
			if (!m_sortSelf)
				report->SortByTotalTime(report->m_root);

			m_frameReportText->SetText(ProfileEntryToStringIndented(report->m_root));
		}

		//Mode text
		std::string modeText = "";
		if (m_treeView)
		{
			modeText.append("V - TREE VIEW");
		}
		else
		{
			if (m_sortSelf)
				modeText.append("V - FLAT VIEW\nL - SORT BY SELF");
			else
				modeText.append("V - FLAT VIEW\nL - SORT BY TOTAL");
		}

		if (m_mouseEnabled)
			modeText.append("\nM - MOUSE ENABLE");
		else
			modeText.append("\nM - MOUSE DISABLE");

		m_viewModeText->SetText(modeText);

		//FPS
		//root is not actual root...we need to get a level down for real info
		m_fpsAndFrameTimeText->SetText(Stringf("FPS: %.0f\nLAST FRAME TIME: %s", std::trunc(1.0 / report->GetTotalFrameTime()), TimePerfCountToString(report->m_frameRoot->m_totalHPC).c_str()));
	}
}

void ProfilerView::Render()
{
	Renderer* r = Renderer::GetInstance();
	Profiler* profiler = Profiler::GetInstance();

	m_canvas->Render();

	profile_measurement_t* worstFrame = profiler->ProfileGetWorstFrameInHistory();

	//needs at least a frame to draw graph
	if (worstFrame != nullptr)
	{
		r->SetShader(nullptr);
		r->SetCamera(m_uiCamera);
		r->DisableDepth();

		//background
		r->DrawAABB(m_graphBounds, Rgba(150, 150, 150, 255));

		MeshBuilder mb;
		mb.SetColor(Rgba::yellow);

		int vertIndex = 0;
		int indexLeft = 0;
		float blockWidth = (float) GRAPH_WIDTH / (PROFILE_MAX_HISTORY_LENGTH - 1);
		profile_measurement_t* leftFrame = nullptr;
		profile_measurement_t* rightFrame = nullptr;
		for (int i = PROFILE_MAX_HISTORY_LENGTH - 1; i > 0; i--)
		{
			leftFrame = profiler->ProfileGetPreviousFrame(i);
			rightFrame = profiler->ProfileGetPreviousFrame(i - 1);

			//no data
			if (leftFrame == nullptr || rightFrame == nullptr)
				break;

			Vector2 bl = Vector2(GRAPH_BOT_LEFT.x + (indexLeft * blockWidth), GRAPH_BOT_LEFT.y);
			Vector2 br = Vector2(GRAPH_BOT_LEFT.x + ((indexLeft + 1) * blockWidth), GRAPH_BOT_LEFT.y);

			float tl_y = RangeMapFloat(leftFrame->GetElapsedTime(), 0, worstFrame->GetElapsedTime(), GRAPH_BOT_LEFT.y, GRAPH_BOT_LEFT.y + GRAPH_HEIGHT);
			Vector2 tl = Vector2(GRAPH_BOT_LEFT.x + (indexLeft * blockWidth), tl_y);

			float tr_y = RangeMapFloat(rightFrame->GetElapsedTime(), 0, worstFrame->GetElapsedTime(), GRAPH_BOT_LEFT.y, GRAPH_BOT_LEFT.y + GRAPH_HEIGHT);
			Vector2 tr = Vector2(GRAPH_BOT_LEFT.x + ((indexLeft + 1) * blockWidth), tr_y);

			Rgba colorToUse;
			if (m_frameSelected + 1 != i)
				colorToUse = Rgba::yellow;
			else
				colorToUse = Rgba::red;

			VertexPCU vertexs[6];
			vertexs[0] = VertexPCU(bl, colorToUse, Vector2(0, 1)); // 0 - (-1, -1)
			vertexs[1] = VertexPCU(br, colorToUse, Vector2(1, 1)); // 1 - (1, -1)
			vertexs[2] = VertexPCU(tl, colorToUse, Vector2(0, 0)); // 2 - (-1, 1)
																		  
			vertexs[3] = VertexPCU(tl, colorToUse, Vector2(0, 0)); // 2 - (-1, 1)
			vertexs[4] = VertexPCU(br, colorToUse, Vector2(1, 1)); // 1 - (1, -1)
			vertexs[5] = VertexPCU(tr, colorToUse, Vector2(1, 0)); // 3 - (1, 1)

			r->BindSampler(0, nullptr);
			r->BindTexture(0, nullptr);
			r->DrawMeshImmediate(vertexs, 6, eDrawPrimitive::TRIANGLES);

 			indexLeft++;
		}

		mb.End();

		r->EnableDepth(COMPARE_LESS, true);
	}

}

void ProfilerView::OnMouseDown(MOUSE_CODE button, const Vector2& mousePos)
{
	//if within graph bounds
	if (m_graphHitBounds.IsPointInside(mousePos))
	{
		if (button == MOUSE_CODE::BUTTON_LEFT)
		{
			m_frameSelected = (int) RangeMapFloat(mousePos.x, GRAPH_BOT_LEFT.x, GRAPH_BOT_LEFT.x + GRAPH_WIDTH, PROFILE_MAX_HISTORY_LENGTH - 1, 0);
			ProfilerPause();
		}
		if (button == MOUSE_CODE::BUTTON_RIGHT)
		{
			ProfilerResume(); 
			m_frameSelected = -1;
		}
	}
}

std::string ProfilerView::ProfileEntryToString(ProfilerReportEntry* entry)
{
	std::string ret = Stringf("%*s%-*s %-6s %-8s %-10s %-8s %-10s\n", 
		0, "", 
		FUNCTION_SPACING, "FUNCTION NAME", 
		"CALLS",
		"% TOTAL",
		"TIME TOTAL", 
		"% SELF", 
		"TIME SELF");

	AppendEntryRecursivelyToString(ret, entry);
	return ret;
}

void ProfilerView::AppendEntryRecursivelyToString(std::string& str, ProfilerReportEntry* entry)
{
	//add yourself
	str.append(
		Stringf("%*s%-*s %-6llu %-8s %-10s %-8s %-10s\n", 
			0, "", 
			FUNCTION_SPACING - 0, entry->m_id.c_str(), 
			entry->m_callCount,
			entry->m_totalPercentStr.c_str(),
			TimePerfCountToString(entry->m_totalHPC).c_str(), 
			entry->m_selfPercentStr.c_str(), 
			TimePerfCountToString(entry->m_selfHPC).c_str()));

	//all your sorted children
	for (std::pair<std::string, ProfilerReportEntry*> it : entry->m_sortedChildren)
	{
		AppendEntryRecursivelyToString(str, it.second);
	}
}

std::string ProfilerView::ProfileEntryToStringIndented(ProfilerReportEntry * entry)
{
	std::string ret = Stringf("%*s%-*s %-6s %-8s %-10s %-8s %-10s\n", 
		0, "", 
		FUNCTION_SPACING, "FUNCTION NAME", 
		"CALLS",
		"% TOTAL",
		"TIME TOTAL", 
		"% SELF", 
		"TIME SELF");

	AppendIndentedEntryRecursivelyToString(ret, entry, 0);
	return ret;
}

void ProfilerView::AppendIndentedEntryRecursivelyToString(std::string& str, ProfilerReportEntry* entry, int depthInTree)
{
	//add yourself
	uint indent = depthInTree * SPACE_PER_INDENT;
	str.append(
		Stringf("%*s%-*s %-6llu %-8s %-10s %-8s %-10s\n", 
			indent, "", 
			FUNCTION_SPACING - indent, entry->m_id.c_str(), 
			entry->m_callCount,
			entry->m_totalPercentStr.c_str(),
			TimePerfCountToString(entry->m_totalHPC).c_str(), 
			entry->m_selfPercentStr.c_str(), 
			TimePerfCountToString(entry->m_selfHPC).c_str()));

	//all your sorted children
	for (std::pair<std::string, ProfilerReportEntry*> it : entry->m_sortedChildren)
	{
		AppendIndentedEntryRecursivelyToString(str, it.second, depthInTree + 1);
	}
}

ProfilerView* ProfilerView::CreateInstance()
{
#if !defined(ENGINE_DISABLE_PROFILING) 
	if (g_profilerView == nullptr) 
	{
		g_profilerView = new ProfilerView();
	}
#endif

	return g_profilerView; 
}

ProfilerView* ProfilerView::GetInstance()
{
	return g_profilerView;
}

void ProfilerView::ProfilerViewToggle(Command& cmd)
{
	UNUSED(cmd);

	if (g_profilerView != nullptr)
		g_profilerView->m_isActive = !g_profilerView->m_isActive;
}
