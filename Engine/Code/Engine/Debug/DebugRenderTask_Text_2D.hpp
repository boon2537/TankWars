#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include <string.h>

class DebugRenderTask_Text_2D : public DebugRenderTask
{
public:
	DebugRenderTask_Text_2D(const AABB2& bounds, float cellHeight, const std::string& asciiText, const Vector2& alignment, const Rgba& tint, eTextDrawMode textDrawMode)
		: DebugRenderTask(),
		m_bounds(bounds),
		m_cellHeight(cellHeight),
		m_asciiText(asciiText),
		m_alignment(alignment),
		m_tint(tint),
		m_textDrawMode(textDrawMode) { m_is2D = true; }

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_overlayCamera);

		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

		r->DrawTextInBox2D(m_bounds, m_asciiText, m_cellHeight, m_tint, 1.f, nullptr, m_textDrawMode, m_alignment);
	}

public:
	AABB2 m_bounds;
	float m_cellHeight;
	std::string m_asciiText;
	Vector2 m_alignment;
	Rgba m_tint;
	eTextDrawMode m_textDrawMode;
};
