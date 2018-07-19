#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/VertexPCU.hpp"
#include "Engine/Renderer/VertexLit.hpp"

class VertexBuffer : public RenderBuffer
{
public:
	uint m_vertexCount;  
	uint m_vertexStride; 
};

class IndexBuffer : public RenderBuffer
{
public:
	uint m_indexStride; 
	uint m_indexCount; 
};

class Mesh
{
public:
	~Mesh();
	Mesh();

	void SetIndices(uint count, const uint* indices);
	const uint GetVertexStride() { return m_layout->m_stride; }

	void SetDrawInstruction(eDrawPrimitive type,
		bool useIndices,
		uint start_index,
		uint elem_count);

	template <typename VERTEX_TYPE>
	void SetVertices(uint count, const VERTEX_TYPE* vertices)
	{
		m_vbo.CopyToGPU(count * sizeof(VERTEX_TYPE), vertices); 
		m_layout = &VERTEX_TYPE::s_layout; 
		m_vbo.m_vertexCount = count;
		m_vbo.m_vertexStride = m_layout->m_stride;
		// this is a form of duck-typing.  Works for any class that has a 
		// s_layout defined that is a VertexLayout;
	}

	template <typename VERTEX_TYPE>
	void FromBuilderForType(const MeshBuilder& mb) 
	{
		uint vcount = (uint) mb.m_vertices.size(); 
		VERTEX_TYPE *temp = (VERTEX_TYPE*) malloc(sizeof(VERTEX_TYPE) * vcount); 

		for (uint i = 0; i < vcount; ++i) 
		{
			// copy each vertex
			temp[i] = VERTEX_TYPE(mb.m_vertices[i]); 
		}

		SetVertices<VERTEX_TYPE>(vcount, temp);
		SetIndices((uint) mb.m_indices.size(), mb.m_indices.data()); 
		m_drawCall = draw_instruction_t(mb.m_draw.m_primitiveType, mb.m_draw.m_startIndex, mb.m_draw.m_elemCount, mb.m_draw.m_usingIndices); 

		// free our temp buffer
		free(temp); 
	}

public:
	// vertices
	VertexBuffer m_vbo; 
	// indices
	IndexBuffer m_ibo; 
	// draw call 
	draw_instruction_t m_drawCall; 

	const VertexLayout* m_layout; // describes the vertices in the mesh

	bool m_isResource = false;

	TODO("aabb3 bounds here");
};
