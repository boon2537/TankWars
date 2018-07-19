#pragma once

#include "Engine/UI/CanvasElement.hpp"
#include "Engine/Core/Rgba.hpp"

class Texture;

class ImageUI : public CanvasElement
{
public:
	~ImageUI() {};
	ImageUI(Canvas* canvas);

	virtual void Render() override;

public:
	Texture* m_texture;
	Rgba m_tint = Rgba::white;
};