#include "Engine/UI/CanvasElement.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/UI/Canvas.hpp"

CanvasElement::CanvasElement(Canvas* canvas)
	: m_canvasAttached(canvas)
{
}

bool CanvasElement::IsActive()
{
	return m_isActive;
}

bool CanvasElement::IsMouseWithinBounds(int x, int y)
{
	return m_screenBounds.IsPointInside(x, y);
}

void CanvasElement::SetActive(bool active)
{
	m_isActive = active;
}

void CanvasElement::SetBounds(const AABB2& bounds)
{
	m_bounds = bounds;

	m_screenBounds.mins.x = RangeMapFloat(m_bounds.mins.x, 0, 1, 0, m_canvasAttached->GetWidth());
	m_screenBounds.maxs.x = RangeMapFloat(m_bounds.maxs.x, 0, 1, 0, m_canvasAttached->GetWidth());
	m_screenBounds.mins.y = RangeMapFloat(m_bounds.mins.y, 0, 1, 0, m_canvasAttached->GetHeight());
	m_screenBounds.maxs.y = RangeMapFloat(m_bounds.maxs.y, 0, 1, 0, m_canvasAttached->GetHeight());
}
