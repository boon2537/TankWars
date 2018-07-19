#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class SpriteAnimSetDefinition
{
friend class SpriteAnimSet;

public:
	SpriteAnimSetDefinition( const tinyxml2::XMLElement& animSetElement, Renderer& renderer );
	~SpriteAnimSetDefinition();

protected:
	std::map< std::string, SpriteAnimDefinition* > m_namedAnimDefs;
	std::string m_defaultAnimName = "Idle";
	//TODO: autoOrient?
};
