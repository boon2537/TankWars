#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Core/XmlUtilities.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition(const tinyxml2::XMLElement& animSetElement, Renderer & renderer)
{
	std::string path = "Data/Images/" + ParseXmlAttribute(animSetElement, "spriteSheet", "empty");
	Ints spriteSheetSize = ParseXmlAttribute(animSetElement, "spriteLayout", Ints());
	SpriteSheet* spriteSheetToUse = new SpriteSheet(*renderer.CreateOrGetTexture(path), spriteSheetSize[0], spriteSheetSize[1]);
	float defaultFps = 10;
	if (animSetElement.FindAttribute("fps") != 0)
		defaultFps = ParseXmlAttribute(animSetElement, "fps", defaultFps);

	const tinyxml2::XMLElement* elementToUse = animSetElement.FirstChildElement();
	while (elementToUse != nullptr)
	{
		SpriteAnimDefinition* def = new SpriteAnimDefinition(*elementToUse, *spriteSheetToUse, defaultFps, renderer); 
		m_namedAnimDefs[def->m_name] = def;
		elementToUse = elementToUse->NextSiblingElement();
	}
}

SpriteAnimSetDefinition::~SpriteAnimSetDefinition()
{
}
