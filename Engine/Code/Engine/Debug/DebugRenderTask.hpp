#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"

enum eDebugRenderMode 
{
	DEBUG_RENDER_IGNORE_DEPTH, // will always draw and be visible 
	DEBUG_RENDER_USE_DEPTH,    // draw using normal depth rules
	DEBUG_RENDER_HIDDEN,       // only draws if it would be hidden by depth
	DEBUG_RENDER_XRAY,         // always draws, but hidden area will be drawn differently
};

struct debug_render_options_t
{
	debug_render_options_t(float liveTime = 0.f, const Rgba& start_color = Rgba::white, const Rgba& end_color = Rgba::white, eDebugRenderMode mode = DEBUG_RENDER_USE_DEPTH)
		: startColor(start_color)
		, endColor(end_color)
		, totalTimeToLive(liveTime)
		, mode(mode)
	{}

	Rgba startColor; 
	Rgba endColor; 
	float totalTimeToLive; 
	eDebugRenderMode mode; 
}; 

class DebugRenderTask
{
public:
	virtual ~DebugRenderTask();
	DebugRenderTask();

	void Age(float deltaSeconds);
	float GetNormalizedAge();
	bool IsDead() { return m_timeToLive >= m_options.totalTimeToLive; }

	virtual void Render() = 0;

public:
	debug_render_options_t m_options;
	float m_timeToLive = 0;

	float m_alphaFactor = 1.f;
	bool m_is2D = false;
};
