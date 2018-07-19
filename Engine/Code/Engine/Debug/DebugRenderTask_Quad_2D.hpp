#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_Quad_2D : public DebugRenderTask
{
public:
	DebugRenderTask_Quad_2D(const AABB2& bounds)
	: DebugRenderTask(),
		m_bounds(bounds) { m_is2D = true; }

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_overlayCamera);

		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

		r->DrawAABB(m_bounds, Rgba::white);
	}

public:
	AABB2 m_bounds;
};
