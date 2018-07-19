#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_Quad : public DebugRenderTask
{
public:
	DebugRenderTask_Quad(const Vector3& position, const Vector3& right, const Vector3& up, const AABB2 bounds, Texture* texture)
		: DebugRenderTask(),
		m_position(position),
		m_right(right),
		m_up(up),
		m_bounds(bounds),
		m_texture(texture) {}

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_3D_camera);

		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

		r->DrawPlane(m_position, m_right, m_up, m_bounds, m_texture, overlayColor);
	}

public:
	Vector3 m_position;
	Vector3 m_right;
	Vector3 m_up;
	AABB2 m_bounds;
	Texture* m_texture;
};
