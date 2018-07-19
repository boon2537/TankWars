#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_Line_2D : public DebugRenderTask
{
public:
	DebugRenderTask_Line_2D(const Vector2& p0, const Vector2& p1, const Rgba& p0_color, const Rgba& p1_color)
		: DebugRenderTask(),
		m_p0(p0),
		m_p1(p1),
		m_p0_color(p0_color),
		m_p1_color(p1_color) { m_is2D = true; }

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_overlayCamera);

		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

		r->DrawLine(m_p0, m_p1, m_p0_color, m_p1_color);
	}

public:
	Vector2 m_p0;
	Vector2 m_p1;
	Rgba m_p0_color;
	Rgba m_p1_color;
};
