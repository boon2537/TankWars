#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

VertexLayout::VertexLayout(uint stride, const vertex_attribute_t* layout)
	: m_stride(stride)
{
	vertex_attribute_t attribute = *layout;
	while (attribute.name != "")
	{
		m_attributes.push_back(attribute);
		layout++;
		attribute = *layout;
	}		
}

uint VertexLayout::GetAttributeCount() const
{
	return (uint) m_attributes.size();
}

vertex_attribute_t const & VertexLayout::GetAttribute(const uint idx) const
{
	ASSERT_RECOVERABLE(idx < GetAttributeCount(), "index out of range");
	return m_attributes[idx];
}
