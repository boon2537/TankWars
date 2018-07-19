#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_WireAABB3 : public DebugRenderTask
{
public:
	DebugRenderTask_WireAABB3(const Vector3& center, const Vector3& dimensions, eFillMode fillMode)
		: DebugRenderTask(),
		m_center(center),
		m_dimensions(dimensions),
		m_fillMode(fillMode) {}

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_3D_camera);

		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

		if (m_fillMode == eFillMode::FILLMODE_SOLID)
			r->SetWireFrameDrawModeActive(false);
		else if (m_fillMode == eFillMode::FILLMODE_WIRE)
			r->SetWireFrameDrawModeActive(true);

		r->DrawCube(m_center, m_dimensions, overlayColor);
	}

public:
	Vector3 m_center;
	Vector3 m_dimensions;
	eFillMode m_fillMode;
};
