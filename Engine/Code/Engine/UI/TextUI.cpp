#include "Engine/UI/TextUI.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/Renderer/Renderer.hpp"

TextUI::TextUI(Canvas* canvas)
	: CanvasElement(canvas)
{
	m_font = canvas->GetDefaultFont();
}

void TextUI::SetText(const std::string& text)
{
	m_text = text;
}

void TextUI::SetFont(BitmapFont* font)
{
	m_font = font;
}

void TextUI::Render()
{
	Renderer* r = Renderer::GetInstance();

	r->DrawTextInBox2D(m_screenBounds, m_text, m_height, m_color, 1.f, m_font, m_drawMode, m_alignment);
}
