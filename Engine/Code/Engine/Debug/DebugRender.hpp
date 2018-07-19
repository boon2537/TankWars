#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <string>
#include <vector>

class Renderer;
class Camera;
class Texture;
class Clock;
class DebugRenderTask_Text_2D;

class DebugRender
{
public:
	~DebugRender();
	DebugRender();

	// Setup the system - allocate whatever internal structures,
	// cameras, and materials are needed
	void DebugRenderStartup(Renderer* r);

	// Cleanup the system
	void DebugRenderShutdown();

	// Called every frame when I want to
	// to have the rendering applied
	void DebugRenderUpdateAndRender();
	void UpdadeDebugLogf(float deltaSeconds);
	void RenderDebugLogf();
	void PostUpdateDebugLogf();

	void DebugRenderClear(); // clear all active draws
	void SetDebugRender3DCamera(Camera* camera); 
	void DebugRenderSetClock(Clock* clock);
	void SetDebugRenderVisible(bool active) { m_visible = active; }
	void ToggleVisibleDebugRender() { m_visible = !m_visible; }

	void InitAndAddTask(DebugRenderTask* task, float lifetime, const Rgba& start_color, const Rgba& end_color, eDebugRenderMode mode);

public:
	static DebugRender* CreateInstance();
	static DebugRender* GetInstance(); 
	static void ConsoleToggleVislble(Command& cmd);
	static void ConsoleClear(Command& cmd);

public:
	Camera* m_overlayCamera = nullptr; //2D camera, drawn to NDC
	Camera* m_3D_camera = nullptr;
	std::vector<DebugRenderTask*> m_debugRenderTasks;
	std::vector<DebugRenderTask_Text_2D*> m_debugLogTasks;

private:
	Clock* m_clock = nullptr;
	float m_lastTime = 0;
	bool m_visible = true;

	ShaderProgram* m_diffuseShader = nullptr; //default shader
};

// Global functions
/************************************************************************/
/* 2D Systems                                                           */
/************************************************************************/
// Note:  These are not really easy to use - they're just all possible
//        options.  Probably good to add helpers that match your common
//        use cases as you find yourself using the system more. 

void DebugRender2DQuad(float lifetime, 
	const AABB2& bounds, 
	const Rgba& start_color, 
	const Rgba& end_color,
	eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH); 

void DebugRender2DLine(float lifetime, 
	const Vector2& p0, const Rgba& p0_color,
	const Vector2& p1, const Rgba& p1_color,
	const Rgba& start_color = Rgba::white,   // tint the overall line
	const Rgba& end_color = Rgba::white,
	eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH);   // tint the overall line

void DebugRender2DText(float lifetime, 
	const AABB2& bounds, 
	float cellHeight, 
	const std::string& asciiText, 
	const Rgba& start_color, 
	const Rgba& end_color, 
	const Vector2& alignment = Vector2(0, 0),
	const Rgba & tint = Rgba::white,
	eTextDrawMode textDrawMode = eTextDrawMode::OVERRUN,
	eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH
	); 

void DebugLogf(const std::string& text, const Rgba& color = Rgba::white, float lifetime = 2.f);

/************************************************************************/
/* 3D Systems                                                           */
/************************************************************************/
void DebugRenderPoint(float lifetime, 
const Vector3& position, 
const Rgba& start_color = Rgba::white, 
const Rgba& end_color = Rgba::white,
eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH); 

void DebugRenderLineSegment(float lifetime, 
const Vector3& p0, const Rgba& p0_color, 
const Vector3& p1, const Rgba& p1_color, 
const Rgba& start_color = Rgba::white, 
const Rgba& end_color = Rgba::white, 
eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH);

void DebugRenderBasis(float lifetime, 
const Matrix44& basis, 
const Rgba& start_color = Rgba::white, 
const Rgba& end_color = Rgba::white, 
eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH); 

void DebugRenderSphere(float lifetime, 
const Vector3& pos, 
float const radius, 
const Rgba& start_color = Rgba::red, 
const Rgba& end_color = Rgba::red, 
eFillMode fillMode = eFillMode::FILLMODE_WIRE,
eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH); 

void DebugRenderAABB3(float lifetime, 
const Vector3& center, 
const Vector3& dimensions,
const Rgba& start_color = Rgba::red, 
const Rgba& end_color = Rgba::red, 
eFillMode fillMode = eFillMode::FILLMODE_WIRE,
eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH); 

void DebugRenderQuad(float lifetime, 
const Vector3& pos, 
const AABB2& bounds,
const Vector3& right,
const Vector3& up,
Texture* texture = nullptr, // default to a white texture if nullptr
const Rgba& start_color = Rgba::white, 
const Rgba& end_color = Rgba::white, 
eDebugRenderMode mode = eDebugRenderMode::DEBUG_RENDER_USE_DEPTH); 
