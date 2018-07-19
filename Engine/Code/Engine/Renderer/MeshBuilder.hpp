#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuilder.hpp"
#include <vector>

struct draw_instruction_t 
{
	draw_instruction_t() {}
	draw_instruction_t(eDrawPrimitive primitiveType, uint startIndex, uint elemCount, bool usingIndices)
		: m_primitiveType(primitiveType), m_startIndex(startIndex), m_elemCount(elemCount), m_usingIndices(usingIndices) {}

	eDrawPrimitive m_primitiveType; 
	uint m_startIndex; 
	uint m_elemCount; 
	bool m_usingIndices; 
};

class Mesh;

class MeshBuilder
{
public:
	~MeshBuilder();
	MeshBuilder();

	void Begin(eDrawPrimitive prim, bool use_indices);
	void End();
	void Reset();

	void SetNormal(const Vector3& normal);
	void SetColor(const Rgba& color);
	void SetUV(const Vector2& uv);
	void SetTangent(const Vector4& tangent);
	void GenerateArbitraryTangents(Vector3* tangent, Vector3* bitangent, Vector3 normal);

	uint PushVertex(const Vector3& position);

	void AddFace(uint first, uint second, uint third);
	void AddQuad(uint p0, uint p1, uint p2, uint p3);

	void AddCube(const Vector3& center, const Vector3& dimensions, const Vector3& scale, const Rgba& tint, AABB2 uv_top = AABB2::ZERO_TO_ONE, AABB2 uv_side = AABB2::ZERO_TO_ONE, AABB2 uv_bottom = AABB2::ZERO_TO_ONE);
	void AddUVSphere(const Vector3& position, float radius, uint wedges, uint slices, const Rgba& tint);
	void AddPlane(const Vector3& center, const Vector3& right, const Vector3& up, const AABB2& bounds, AABB2 uv = AABB2::ZERO_TO_ONE, const Rgba& tint = Rgba::white);
	void AddSprite(const Vector3& center, const Vector2& extends, const Vector3& scale = Vector3::one, const Rgba& tint = Rgba::white, AABB2 uv = AABB2::ZERO_TO_ONE);
	void AddFromObjFile(const std::string& path);

public:	
	Vertex_builder m_stamp;
	std::vector<Vertex_builder> m_vertices;
	std::vector<uint> m_indices;

	draw_instruction_t m_draw; 
};
