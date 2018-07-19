#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/EngineCommon.hpp"

enum SPRITE_ANIM_MODE
{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
	NUM_SPRITE_ANIM_MODES
};

class SpriteAnimDefinition
{
friend class SpriteAnim;
friend class SpriteAnimSetDefinition;

public:
	SpriteAnimDefinition( const tinyxml2::XMLElement& animElement,
		const SpriteSheet& defaultSpriteSheet, 
		float defaultFps, Renderer& renderer );

	~SpriteAnimDefinition();

	float GetDuration() const { return (float) m_spriteIndexes.size() / m_framesPerSecond; }
	int GetSpriteIndexAtTime( float elapsedSeconds ) const;

protected:
	std::string m_name;
	const SpriteSheet* m_spriteSheet = nullptr;
	float m_framesPerSecond = 10.f;
	Ints m_spriteIndexes;
	bool m_shouldAutoOrient = false;
	SPRITE_ANIM_MODE m_mode = SPRITE_ANIM_MODE_LOOPING; //TODO: read and store from xml later
};
