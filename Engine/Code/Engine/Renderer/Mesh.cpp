#include "Mesh.hpp"

Mesh::~Mesh()
{
}

Mesh::Mesh()
{
}

void Mesh::SetIndices(uint count, const uint* indices)
{
	m_ibo.CopyToGPU(sizeof(uint) * count, indices);
	m_ibo.m_indexCount = count;
	m_ibo.m_indexStride = sizeof(uint);
}

void Mesh::SetDrawInstruction(eDrawPrimitive type, bool useIndices, uint start_index, uint elem_count)
{
	m_drawCall = draw_instruction_t(type, useIndices, start_index, elem_count);
}
