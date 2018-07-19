#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include <vector>
#include <string>

struct vertex_attribute_t
{
	vertex_attribute_t() {}
	vertex_attribute_t(const char* name, eRenderDataType type, uint elemCount, eRenderDataType isNormalized, size_t memberOffset)
		: type(type), elemCount(elemCount), isNormalized(isNormalized), memberOffset(memberOffset)
	{
		this->name = std::string(name);
	}

	std::string name = "";       // because we have to search for it
	eRenderDataType type;  // what? (abstract away GL from the header)
	uint        elemCount; // how many?
	eRenderDataType	isNormalized; 
	size_t        memberOffset; 
};

// A place to collect all the attributes.
class VertexLayout 
{
public:
	VertexLayout(uint stride, const vertex_attribute_t* layout);

	uint GetAttributeCount() const; 
	const vertex_attribute_t& GetAttribute(const uint idx) const;  

public:
	// what are the members
	std::vector<vertex_attribute_t> m_attributes;

	// how large is the vertex in bytes
	uint m_stride; 
};
