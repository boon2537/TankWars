#include "Engine/Renderer/SpriteSheet.hpp"

SpriteSheet::SpriteSheet(const Texture & texture, int tilesWide, int tilesHigh)
	: m_spriteSheetTexture(texture)
	, m_spriteLayout(IntVector2(tilesWide, tilesHigh))
{}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(const IntVector2 & spriteCoords) const
{
	float xCords = (float) spriteCoords.x / m_spriteLayout.x;
	float yCords = (float) spriteCoords.y / m_spriteLayout.y;
	return AABB2(xCords, m_spriteLayout.y - yCords, xCords + (1.f / m_spriteLayout.x), m_spriteLayout.y - (yCords + (1.f / m_spriteLayout.y)));
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
	int xConverted = spriteIndex % m_spriteLayout.x;
	int yConverted = spriteIndex / m_spriteLayout.x;
	float xCords = (float) xConverted / m_spriteLayout.x;
	float yCords = (float) yConverted / m_spriteLayout.y;
	return AABB2(xCords, m_spriteLayout.y - yCords, xCords + (1.f / m_spriteLayout.x), m_spriteLayout.y - (yCords + (1.f / m_spriteLayout.y)));
}

int SpriteSheet::GetNumSprites() const
{
	return m_spriteLayout.x * m_spriteLayout.y;
}

const Texture& SpriteSheet::GetTexture() const
{
	return m_spriteSheetTexture;
}
