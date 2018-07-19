#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_Sphere : public DebugRenderTask
{
public:
	DebugRenderTask_Sphere(const Vector3& position, float radius, eFillMode fillMode)
		: DebugRenderTask(),
		m_position(position),
		m_radius(radius),
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

		r->DrawUVSphere(m_position, m_radius, Rgba::white);
	}

public:
	Vector3 m_position;
	float m_radius;
	eFillMode m_fillMode;
};
