#include "Engine/UI/ImageUI.hpp"
#include "Engine/Renderer/Renderer.hpp"

ImageUI::ImageUI(Canvas* canvas)
	: CanvasElement(canvas)
{
}

void ImageUI::Render()
{
	Renderer* r = Renderer::GetInstance();
	if (m_texture != nullptr)
	{	
		r->DrawTexturedAABB(m_screenBounds, *m_texture, Vector2::zero, Vector2::one, m_tint);
	}
	else
	{
		r->DrawAABB(m_screenBounds, m_tint);
	}
}
