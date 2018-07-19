#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/RendererTypes.hpp"

#include "Engine/Debug/DebugRenderTask_Line_2D.hpp"
#include "Engine/Debug/DebugRenderTask_Quad_2D.hpp"
#include "Engine/Debug/DebugRenderTask_Text_2D.hpp"
#include "Engine/Debug/RenderDebugTask_Basis.hpp"
#include "Engine/Debug/RenderDebugTask_LineSegment.hpp"
#include "Engine/Debug/RenderDebugTask_Point.hpp"
#include "Engine/Debug/RenderDebugTask_Quad.hpp"
#include "Engine/Debug/RenderDebugTask_Sphere.hpp"
#include "Engine/Debug/RenderDebugTask_WireAABB3.hpp"

constexpr float DEBUG_LOG_HEIGHT = 18.f;
constexpr float DEBUG_LOG_HEIGHT_BUFFER = 4.f;
constexpr float DEBUG_LOG_X_OFFSET = 5.f;

static DebugRender* g_DebugRender = nullptr; 

DebugRender::~DebugRender()
{
	DebugRenderClear();
}

DebugRender::DebugRender()
{
}

void DebugRender::DebugRenderStartup(Renderer* r)
{
	m_overlayCamera = new Camera();
	m_overlayCamera->SetProjection(Matrix44::MakeOrtho(Vector2(0, 0), Vector2((float) Window::GetWidth(), (float) Window::GetHeight()), 0, 1));
	m_overlayCamera->SetColorTarget(r->GetDefaultColorTarget());
	m_overlayCamera->SetDepthStencilTarget(r->GetDefaultDepthTarget());

	m_diffuseShader = r->CreateOrGetShaderProgram("default");

	CommandRegister("debug_toggle", DebugRender::ConsoleToggleVislble, "Toggle debug render.");
	CommandRegister("debug_clear", DebugRender::ConsoleClear, "Clear debug render.");
}

void DebugRender::DebugRenderShutdown()
{
	delete m_overlayCamera;
}

void DebugRender::DebugRenderUpdateAndRender()
{
	float currentTime = static_cast<float>(m_clock->GetClockCurrentTime());	
	float deltaSeconds = currentTime - m_lastTime;
	m_lastTime = currentTime;

	Renderer* r = Renderer::GetInstance();

	for each (DebugRenderTask* task in m_debugRenderTasks)
	{
		task->Age(deltaSeconds);
	}

	UpdadeDebugLogf(deltaSeconds);

	if (m_visible)
	{
		r->SetShader(nullptr);

		for each (DebugRenderTask* task in m_debugRenderTasks)
		{
			if (!task->m_is2D)
			{
				if (task->m_options.mode == eDebugRenderMode::DEBUG_RENDER_HIDDEN)
				{
					r->EnableDepth(COMPARE_GREATER, false);
				}
				else if (task->m_options.mode == eDebugRenderMode::DEBUG_RENDER_IGNORE_DEPTH)
				{
					r->DisableDepth();
				}
				else if (task->m_options.mode == eDebugRenderMode::DEBUG_RENDER_XRAY)
				{
					//for xray part
					r->EnableDepth(COMPARE_GREATER, false); 
					task->m_alphaFactor = 0.4f;
					task->Render();

					task->m_alphaFactor = 1.f;
					r->EnableDepth(COMPARE_LESS, true); 
				}
				else
				{
					r->EnableDepth(COMPARE_LESS, true); 	
				}

				task->Render();
				//reset possible wireframe draw mode
				r->SetWireFrameDrawModeActive(false);
			}
		}

		//to ensure 2D elements are drawn on top
		r->DisableDepth();
		for each (DebugRenderTask* task in m_debugRenderTasks)
		{
			if (task->m_is2D)
			{
				task->Render();
			}
		}

		RenderDebugLogf();

		//reset depth setting
		r->EnableDepth(COMPARE_LESS, true); 
		//reset tint
		r->SetUniform("TINT", Vector4(1.f, 1.f, 1.f, 1.f));
	}

	for (int task_index = 0; task_index < m_debugRenderTasks.size(); ++task_index)
	{
		DebugRenderTask* task = m_debugRenderTasks[task_index];
		if (task->IsDead())
		{
			delete task;

			size_t size = m_debugRenderTasks.size();
			m_debugRenderTasks[task_index] = m_debugRenderTasks[size - 1];
			m_debugRenderTasks.pop_back();
			task_index--;
		}
	}

	PostUpdateDebugLogf();
}

void DebugRender::UpdadeDebugLogf(float deltaSeconds)
{
	float width = (float) Window::GetWidth();
	float topAnchor = (float) Window::GetHeight();
	float cellHeight = DEBUG_LOG_HEIGHT + DEBUG_LOG_HEIGHT_BUFFER;

	for each (DebugRenderTask_Text_2D* task in m_debugLogTasks)
	{
		task->Age(deltaSeconds);
		task->m_bounds = AABB2(DEBUG_LOG_X_OFFSET, topAnchor - cellHeight, width, topAnchor);
		topAnchor -= cellHeight;
	}
}

void DebugRender::RenderDebugLogf()
{
	for each (DebugRenderTask_Text_2D* task in m_debugLogTasks)
	{
		task->Render();
	}
}

void DebugRender::PostUpdateDebugLogf()
{
	//clean up
	for (int task_index = 0; task_index < m_debugLogTasks.size(); ++task_index)
	{
		DebugRenderTask* task = m_debugLogTasks[task_index];
		if (task->IsDead())
		{
			delete task;

			size_t size = m_debugLogTasks.size();
			m_debugLogTasks[task_index] = m_debugLogTasks[size - 1];
			m_debugLogTasks.pop_back();
			task_index--;
		}
	}
}

void DebugRender::DebugRenderClear()
{
	for (int task_index = 0; task_index < m_debugRenderTasks.size(); ++task_index)
		delete m_debugRenderTasks[task_index];

	for (int log_index = 0; log_index < m_debugLogTasks.size(); ++log_index)
		delete m_debugLogTasks[log_index];

	m_debugRenderTasks.clear();
	m_debugLogTasks.clear();
}

void DebugRender::SetDebugRender3DCamera(Camera* camera)
{
	m_3D_camera = camera;
}

void DebugRender::DebugRenderSetClock(Clock* clock)
{
	m_clock = clock;
}

void DebugRender::InitAndAddTask(DebugRenderTask* task, float lifetime, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	task->m_options = debug_render_options_t(lifetime, start_color, end_color, mode);
	g_DebugRender->m_debugRenderTasks.push_back(task);
}

DebugRender* DebugRender::CreateInstance()
{
	if (g_DebugRender == nullptr) 
	{
		g_DebugRender = new DebugRender(); 
	}
	return g_DebugRender; 
}

DebugRender* DebugRender::GetInstance()
{
	return g_DebugRender;
}

void DebugRender::ConsoleToggleVislble(Command& cmd)
{
	g_DebugRender->ToggleVisibleDebugRender();
	UNUSED(cmd);
}

void DebugRender::ConsoleClear(Command& cmd)
{
	g_DebugRender->DebugRenderClear();
	UNUSED(cmd);
}

void DebugRender2DQuad(float lifetime, const AABB2& bounds, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	DebugRenderTask_Quad_2D* task = new DebugRenderTask_Quad_2D(bounds);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRender2DLine(float lifetime, const Vector2& p0, const Rgba& p0_color, const Vector2& p1, const Rgba& p1_color, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	DebugRenderTask_Line_2D* task = new DebugRenderTask_Line_2D(p0, p1, p0_color, p1_color);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRender2DText(float lifetime, const AABB2& bounds, float cellHeight, const std::string& asciiText, const Rgba& start_color, const Rgba& end_color, const Vector2& alignment, const Rgba& tint, eTextDrawMode textDrawMode, eDebugRenderMode mode)
{
	DebugRenderTask_Text_2D* task = new DebugRenderTask_Text_2D(bounds, cellHeight, asciiText, alignment, tint, textDrawMode);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugLogf(const std::string& text, const Rgba& color, float lifetime)
{
	DebugRenderTask_Text_2D* task = new DebugRenderTask_Text_2D(AABB2::ZERO_TO_ONE, DEBUG_LOG_HEIGHT, text, Vector2(0, 0.5f), color, eTextDrawMode::OVERRUN);
	task->m_options = debug_render_options_t(lifetime, Rgba::white, Rgba::white, eDebugRenderMode::DEBUG_RENDER_USE_DEPTH);
	g_DebugRender->m_debugLogTasks.push_back(task);
}

void DebugRenderPoint(float lifetime, const Vector3& position, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	DebugRenderTask_Point* task = new DebugRenderTask_Point(position);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRenderLineSegment(float lifetime, const Vector3& p0, const Rgba& p0_color, const Vector3& p1, const Rgba& p1_color, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	DebugRenderTask_LineSegment* task = new DebugRenderTask_LineSegment(p0, p1, p0_color, p1_color);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRenderBasis(float lifetime, const Matrix44& basis, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	DebugRenderTask_Basis* task = new DebugRenderTask_Basis(basis);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRenderSphere(float lifetime, const Vector3& pos, float const radius, const Rgba& start_color, const Rgba& end_color, eFillMode fillMode, eDebugRenderMode mode)
{
	DebugRenderTask_Sphere* task = new DebugRenderTask_Sphere(pos, radius, fillMode);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRenderAABB3(float lifetime, const Vector3& center, const Vector3& dimensions, const Rgba& start_color, const Rgba& end_color, eFillMode fillMode, eDebugRenderMode mode)
{
	DebugRenderTask_WireAABB3* task = new DebugRenderTask_WireAABB3(center, dimensions, fillMode);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}

void DebugRenderQuad(float lifetime, const Vector3& pos, const AABB2& bounds, const Vector3& right, const Vector3& up, Texture* texture, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode)
{
	DebugRenderTask_Quad* task = new DebugRenderTask_Quad(pos, right, up, bounds, texture);
	g_DebugRender->InitAndAddTask(task, lifetime, start_color, end_color, mode);
}
