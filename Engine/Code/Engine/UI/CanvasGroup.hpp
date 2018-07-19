#pragma once

#include "Engine/UI/CanvasElement.hpp"
#include <vector>

class Canvas;

class CanvasGroup
{
public:
	~CanvasGroup() {};
	CanvasGroup(Canvas* canvas);

public:
	Canvas* m_canvasAttached = nullptr;

	bool m_isActive = true;
	std::vector<CanvasElement*> m_elements;
};
