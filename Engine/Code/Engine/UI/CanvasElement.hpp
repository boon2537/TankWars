#pragma once

#include "Engine/Math/AABB2.hpp"

class Canvas;

class CanvasElement
{
public:
	virtual ~CanvasElement() {};
	CanvasElement(Canvas* canvas);

	bool IsActive();
	bool IsMouseWithinBounds(int x, int y);
	void SetActive(bool active);
	void SetBounds(const AABB2& bounds);

	virtual void Render() = 0;
	virtual bool MouseDownInteract(int screenX, int screenY) { return false; };
	virtual bool MouseUpInteract(int screenX, int screenY) { return false; };
	virtual void OnClicked() {};

protected:
	Canvas* m_canvasAttached = nullptr;

	bool m_isActive = true;
	AABB2 m_bounds;
	AABB2 m_screenBounds;
};
