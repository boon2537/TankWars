#include "Engine/Renderer/Sprite.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <map>

static std::map<std::string, Sprite*> g_spriteMap;

Sprite::Sprite(std::string name, Texture* texture, const AABB2& uv, const Vector2& pivot, const Rgba& tint, const Vector2& scale)
	: m_name(name),
	m_texture(texture),
	m_uv(uv),
	m_pivot(pivot),
	m_tint(tint),
	m_scale(scale)
{
	Vector2 uv_dimensions = m_uv.GetDimensions();
	float width = uv_dimensions.x / uv_dimensions.y;
	m_dimensions = Vector2(width, 1);
}

AABB2 Sprite::GetNormalizedUV()
{
	//assume pixel mode for now
	float minX = (float) m_uv.mins.x / m_texture->GetWidth();
	float maxX = (float) m_uv.maxs.x / m_texture->GetWidth();
	float minY = (float) m_uv.mins.y / m_texture->GetHeight();
	float maxY = (float) m_uv.maxs.y / m_texture->GetHeight();

	//flip
	return AABB2(minX, 1 - maxY, maxX, 1 - minY);
}

Sprite* Sprite::RegisterSprite(std::string name, Texture* texture, const AABB2& uv, const Vector2& pivot, const Rgba& tint, const Vector2& scale)
{
	std::map<std::string, Sprite*>::iterator search = g_spriteMap.find(name);
	if (search != g_spriteMap.end())
	{
		return search->second;
	}
	else
	{
		Sprite* loadedSprite = new Sprite(name, texture, uv, pivot, tint, scale);
		g_spriteMap.insert(std::pair<std::string, Sprite*>(name, loadedSprite));
		return loadedSprite;
	}

	return nullptr;
}

Sprite* Sprite::RegisterSprite(const tinyxml2::XMLElement& spriteElement, const Vector2& scale)
{
	std::string name = ParseXmlAttribute(spriteElement, "id", "");
	std::map<std::string, Sprite*>::iterator search = g_spriteMap.find(name);
	if (search != g_spriteMap.end())
	{
		ASSERT_RECOVERABLE(false, "Attemped to add repeated Sprite entry");
		return search->second;
	}

	Texture* texture = nullptr;
	AABB2 uv = AABB2::ZERO_TO_ONE;
	Vector2 pivot = Vector2(0.5f, 0.5f);
	Rgba tint = Rgba::white;

	const tinyxml2::XMLElement* elementToUse = spriteElement.FirstChildElement();
	while (elementToUse != nullptr)
	{
		std::string elementName = elementToUse->Name();

		if (elementName == "diffuse")
		{
			std::string path = ParseXmlAttribute(*elementToUse, "src", "");
			texture = Renderer::GetInstance()->CreateOrGetTexture(path);
		}
		if (elementName == "uv")
		{
			uv = ParseXmlAttribute(*elementToUse, "uvs", uv);
		}
		if (elementName == "pivot")
		{
			pivot = ParseXmlAttribute(*elementToUse, "xy", pivot);
		}
		if (elementName == "tint")
		{
			tint = ParseXmlAttribute(*elementToUse, "color", tint);
		}

		elementToUse = elementToUse->NextSiblingElement();
	}

	Sprite* loadedSprite = new Sprite(name, texture, uv, pivot, tint, scale);
	g_spriteMap.insert(std::pair<std::string, Sprite*>(name, loadedSprite));
	return loadedSprite;
}

void Sprite::LoadSpritesXML(std::string path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());

	tinyxml2::XMLElement* elementToUse = doc.RootElement()->FirstChildElement();
	while (elementToUse != nullptr)
	{
		RegisterSprite(*elementToUse);
		elementToUse = elementToUse->NextSiblingElement();
	}
}

Sprite* Sprite::GetSprite(std::string name)
{
	std::map<std::string, Sprite*>::iterator search = g_spriteMap.find(name);
	if (search != g_spriteMap.end())
	{
		return search->second;
	}

	return nullptr;
}
