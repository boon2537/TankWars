#pragma once

#include "Engine/UI/CanvasGroup.hpp"
#include <vector>

class Camera;
class BitmapFont;

class Canvas 
{
public: 
	~Canvas();
	Canvas();

	bool IsMouseBlocked(int screenX, int screenY);

	void SetUICamera(Camera* camera);
	void OnMouseDown(int screenX, int screenY);
	void OnMouseUp(int screenX, int screenY);
	void Render();

	int GetWidth();
	int GetHeight();

	void SetDefaultFont(BitmapFont* font);
	inline BitmapFont* GetDefaultFont() { return m_defaultFont; };

public:
	std::vector<CanvasGroup*> m_canvasGroups;

private:
	Camera* m_UICamera = nullptr;
	int m_screenWidth;
	int m_screenHeight;

	CanvasElement* m_selectedElement = nullptr;
	BitmapFont* m_defaultFont = nullptr;
};
