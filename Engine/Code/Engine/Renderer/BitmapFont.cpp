#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/EngineCommon.hpp"

BitmapFont::BitmapFont(const std::string& fontName, const SpriteSheet& glyphSheet, float baseAspect)
	: m_fontName(fontName)
	, m_spriteSheet(glyphSheet)
	, m_baseAspect(baseAspect)	
{

}

AABB2 BitmapFont::GetUVsForGlyph(int glyphUnicode) const
{
	return m_spriteSheet.GetTexCoordsForSpriteIndex(glyphUnicode);
}

float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);

	return m_baseAspect;
}

float BitmapFont::GetStringWidth(const std::string & asciiText, float cellHeight, float aspectScale) const
{
	return asciiText.length() * cellHeight * aspectScale;
}
