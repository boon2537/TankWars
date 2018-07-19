#pragma once

#include "Engine/Renderer/UniformBuffer.hpp"
#include <vector>

/*class PropertyBlockInfo 
{
public:
	std::vector<PropertyInfo> m_infos; 
	size_t maxSize; 
};

class PropertyInfo 
{
public:
	PropertyBlockInfo *m_owningBlock; 

	std::string name; 
	size_t offset; 
	size_t size; 
	size_t count; // array > 1;
};

class ShaderProgramInfo
{
public:
	const PropertyBlockInfo* FindBlockInfo(const char* block_name); 
	const PropertyBlockInfo* FindContainingBlock(const char* prop_name); 
	const PropertyInfo* FindInfo(const char* name); 

public:
	std::vector<PropertyBlockInfo> m_block_infos; 
};*/

class PropertyBlock : public UniformBuffer
{
public:
	//const PropertyBlockInfo* m_info; 
};
