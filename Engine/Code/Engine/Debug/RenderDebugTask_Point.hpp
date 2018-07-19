#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_Point : public DebugRenderTask
{
public:
	DebugRenderTask_Point(const Vector3& position)
		: DebugRenderTask(),
		m_position(position) {}

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_3D_camera);

		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

// 		r->DrawLine(m_position, m_position + Vector3::right, Rgba::blue);
// 		r->DrawLine(m_position, m_position + Vector3::forward, Rgba::red);
// 		r->DrawLine(m_position, m_position + Vector3::up, Rgba::green);

		r->DrawLine(m_position - Vector3::right, m_position + Vector3::right, Rgba::white);
		r->DrawLine(m_position - Vector3::forward, m_position + Vector3::forward, Rgba::white);
		r->DrawLine(m_position - Vector3::up, m_position + Vector3::up, Rgba::white);
	}

public:
	Vector3 m_position;
};
