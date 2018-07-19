#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include <string>

class Texture;

//A simple data class (for now) to store what it takes to easily draw a 2D image

class Sprite
{
public:
	Sprite(std::string name, Texture* texture = nullptr, const AABB2& uv = AABB2::ZERO_TO_ONE, 
		const Vector2& pivot = Vector2(0.5f, 0.5f), const Rgba& tint = Rgba::white, const Vector2& scale = Vector2(1, 1));

	AABB2 GetNormalizedUV();

	static Sprite* RegisterSprite(std::string name, Texture* texture = nullptr, const AABB2& uv = AABB2::ZERO_TO_ONE, 
		const Vector2& pivot = Vector2(0.5f, 0.5f), const Rgba& tint = Rgba::white, const Vector2& scale = Vector2(1, 1));

	static Sprite* RegisterSprite(const tinyxml2::XMLElement& spriteElement, const Vector2& scale = Vector2(1, 1));
	static void LoadSpritesXML(std::string path);
	static Sprite* GetSprite(std::string name);

public:
	std::string m_name;
	Texture* m_texture = nullptr;
	Vector2 m_dimensions;
	AABB2 m_uv;
	Vector2 m_pivot;
	Vector2 m_scale;
	Rgba m_tint;
};
