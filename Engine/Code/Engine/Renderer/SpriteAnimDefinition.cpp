#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition(const tinyxml2::XMLElement& animElement, const SpriteSheet& defaultSpriteSheet, float defaultFps, Renderer& renderer)
{
	UNUSED(renderer);

	m_name = ParseXmlAttribute(animElement, "name", "Empty");
	m_spriteSheet = &defaultSpriteSheet; //TODO: make it so you can specify sprite sheet later?
	if (animElement.FindAttribute("fps") != 0)
		m_framesPerSecond = ParseXmlAttribute(animElement, "fps", defaultFps);
	else
		m_framesPerSecond = defaultFps;
	m_spriteIndexes = ParseXmlAttribute(animElement, "spriteIndexes", m_spriteIndexes);

	if (animElement.FindAttribute("autoOrient") != 0)
		m_shouldAutoOrient = ParseXmlAttribute(animElement, "autoOrient", m_shouldAutoOrient);

	std::string mode = ParseXmlAttribute(animElement, "mode", "loop");
	if (mode == "loop")
		m_mode = SPRITE_ANIM_MODE::SPRITE_ANIM_MODE_LOOPING;
	else if (mode == "once")
		m_mode = SPRITE_ANIM_MODE::SPRITE_ANIM_MODE_PLAY_TO_END;
}

SpriteAnimDefinition::~SpriteAnimDefinition()
{
}

int SpriteAnimDefinition::GetSpriteIndexAtTime(float elapsedSeconds) const
{
	int frame = static_cast<int>( m_spriteIndexes.size() * (elapsedSeconds / GetDuration()));
	return m_spriteIndexes[MinInt(frame, (int) m_spriteIndexes.size() - 1)];
}
