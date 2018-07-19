#pragma once

#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRender.hpp"

class DebugRenderTask_Basis : public DebugRenderTask
{
public:
	DebugRenderTask_Basis(const Matrix44& basis)
		: DebugRenderTask(),
		m_basis(basis) {}

	virtual void Render() override
	{
		Renderer* r = Renderer::GetInstance();
		DebugRender* dr = DebugRender::GetInstance();

		r->SetCamera(dr->m_3D_camera);

		Vector3 origin = m_basis.GetPosition();
		Rgba overlayColor = Interpolate(m_options.endColor, m_options.startColor, GetNormalizedAge());
		overlayColor.a = static_cast<unsigned char>((float) overlayColor.a * m_alphaFactor);
		r->SetUniform("TINT", overlayColor);

		r->DrawLine(origin, origin + (m_basis.GetRight() * 6), Rgba::red);
		r->DrawLine(origin, origin + (m_basis.GetForward() * 6), Rgba::blue);
		r->DrawLine(origin, origin + (m_basis.GetUp()  * 6), Rgba::green);
	}

public:
	Matrix44 m_basis;
};
