#include "Engine/Renderer/VertexPCU.hpp"

const vertex_attribute_t VertexPCU::s_attributes[] = {
	vertex_attribute_t("POSITION",	RDT_FLOAT,			3, RDT_FALSE, offsetof(VertexPCU, m_position)),
	vertex_attribute_t("COLOR",		RDT_UNSIGNED_BYTE,	4, RDT_TRUE, offsetof(VertexPCU, m_color)),
	vertex_attribute_t("UV",		RDT_FLOAT,			2, RDT_FALSE, offsetof(VertexPCU, m_UVs)),
	vertex_attribute_t() // like a null terminator, how do we know the list is done;
};

const VertexLayout VertexPCU::s_layout = VertexLayout(sizeof(VertexPCU), VertexPCU::s_attributes);
