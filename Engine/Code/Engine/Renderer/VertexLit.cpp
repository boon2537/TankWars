#include "Engine/Renderer/VertexLit.hpp"

const vertex_attribute_t VertexLit::s_attributes[] = {
	vertex_attribute_t("POSITION",	RDT_FLOAT,			3, RDT_FALSE, offsetof(VertexLit, m_position)),
	vertex_attribute_t("NORMAL",	RDT_FLOAT,			3, RDT_TRUE, offsetof(VertexLit, m_normal)),
	vertex_attribute_t("COLOR",		RDT_UNSIGNED_BYTE,	4, RDT_TRUE, offsetof(VertexLit, m_color)),
	vertex_attribute_t("UV",		RDT_FLOAT,			2, RDT_FALSE, offsetof(VertexLit, m_UVs)),
	vertex_attribute_t("TANGENT",	RDT_FLOAT,			4, RDT_TRUE, offsetof(VertexLit, m_tangent)),
	vertex_attribute_t() // like a null terminator, how do we know the list is done;
};

const VertexLayout VertexLit::s_layout = VertexLayout(sizeof(VertexLit), VertexLit::s_attributes);
