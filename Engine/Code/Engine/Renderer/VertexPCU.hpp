#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Renderer/VertexBuilder.hpp"
#include <string>
#include <vector>

struct VertexPCU
{
public:
	VertexPCU() {}
	explicit VertexPCU(const Vector3& initialPosition, Rgba initialColor = Rgba::white, Vector2 initialUVs = Vector2(0, 0))
		: m_position(initialPosition)
		, m_color(initialColor)
		, m_UVs(initialUVs) {}
	explicit VertexPCU(const Vector2& initialPosition, Rgba initialColor = Rgba::white, Vector2 initialUVs = Vector2(0, 0))
		: m_position(Vector3(initialPosition.x, initialPosition.y, 0))
		, m_color(initialColor)
		, m_UVs(initialUVs) {}

	VertexPCU(const Vertex_builder& other)
	{
		m_position = other.position; 
		m_color = other.color; 
		m_UVs = other.UV; 
	}

public:
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_UVs;

	static const vertex_attribute_t s_attributes[];
	static const VertexLayout s_layout;
};
