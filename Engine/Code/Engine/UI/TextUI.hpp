#pragma once

#include "Engine/UI/CanvasElement.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include <string>

class BitmapFont;

class TextUI : public CanvasElement
{
public:
	~TextUI() {};
	TextUI(Canvas* canvas);

	void SetText(const std::string& text);
	void SetFont(BitmapFont* font);
	virtual void Render() override;

public:
	std::string m_text;
	BitmapFont* m_font = nullptr;
	Rgba m_color = Rgba::white;
	eTextDrawMode m_drawMode = OVERRUN;
	Vector2 m_alignment = Vector2::zero;
	float m_height = 10.f;
};