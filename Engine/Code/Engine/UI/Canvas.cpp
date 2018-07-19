#include "Engine/UI/Canvas.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Window.hpp"

Canvas::~Canvas()
{
}

Canvas::Canvas()
{
	m_screenWidth = Window::GetWidth();
	m_screenHeight = Window::GetHeight();
}

bool Canvas::IsMouseBlocked(int screenX, int screenY)
{
	for each (CanvasGroup* group in m_canvasGroups)
	{
		if (group->m_isActive)
		{
			for each (CanvasElement* element in group->m_elements)
			{
				if (element->IsActive() && element->IsMouseWithinBounds(screenX, m_screenHeight - screenY))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void Canvas::SetUICamera(Camera* camera)
{
	m_UICamera = camera;
	m_UICamera->SetProjection(Matrix44::MakeOrtho(Vector2(0, 0), Vector2((float) m_screenWidth, (float) m_screenHeight), 0, 1));
}

void Canvas::OnMouseDown(int screenX, int screenY)
{
	for each (CanvasGroup* group in m_canvasGroups)
	{
		if (group->m_isActive)
		{
			for each (CanvasElement* element in group->m_elements)
			{
				if (element->IsActive() && element->MouseDownInteract(screenX, m_screenHeight - screenY))
				{
					m_selectedElement = element;
				}
			}
		}
	}
}

void Canvas::OnMouseUp(int screenX, int screenY)
{
	CanvasElement* potential = nullptr;

	for each (CanvasGroup* group in m_canvasGroups)
	{
		if (group->m_isActive)
		{
			for each (CanvasElement* element in group->m_elements)
			{
				if (element->IsActive() && element->MouseUpInteract(screenX, m_screenHeight - screenY))
				{
					potential = element;
				}
			}
		}
	}

	if (m_selectedElement != nullptr && m_selectedElement == potential)
	{
		m_selectedElement->OnClicked();
	}

	m_selectedElement = nullptr;
}

void Canvas::Render()
{
	Renderer* r = Renderer::GetInstance();
	r->SetShader(nullptr);
	r->SetCamera(m_UICamera);
	r->DisableDepth();

	for each (CanvasGroup* group in m_canvasGroups)
	{
		if (group->m_isActive)
		{
			for each (CanvasElement* element in group->m_elements)
			{
				if (element->IsActive())
					element->Render();
			}
		}
	}

	//reset depth setting
	r->EnableDepth(COMPARE_LESS, true); 
}

int Canvas::GetWidth()
{
	return m_screenWidth;
}

int Canvas::GetHeight()
{
	return m_screenHeight;
}

void Canvas::SetDefaultFont(BitmapFont* font)
{
	m_defaultFont = font;
}
