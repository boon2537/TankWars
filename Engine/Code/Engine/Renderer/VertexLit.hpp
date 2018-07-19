#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Renderer/VertexBuilder.hpp"
#include "Engine/Math/Vector4.hpp"
#include <string>
#include <vector>

struct VertexLit
{
public:
	VertexLit() {}
	explicit VertexLit(const Vector3& initialPosition, const Vector3& normal, Rgba initialColor = Rgba::white, Vector2 initialUVs = Vector2(0, 0), Vector4 tangent = Vector4(1, 1, 1, 1))
		: m_position(initialPosition),
		m_normal(normal),
		m_color(initialColor),
		m_UVs(initialUVs),
		m_tangent(tangent) {}
	explicit VertexLit(const Vector2& initialPosition, const Vector3& normal, Rgba initialColor = Rgba::white, Vector2 initialUVs = Vector2(0, 0), Vector4 tangent = Vector4(1, 1, 1, 1))
		: m_position(Vector3(initialPosition.x, initialPosition.y, 0)),
		m_normal(normal),
		m_color(initialColor),
		m_UVs(initialUVs),
		m_tangent(tangent) {}

	VertexLit(const Vertex_builder& other)
	{
		m_position = other.position; 
		m_normal = other.normal;
		m_color = other.color; 
		m_UVs = other.UV; 
		m_tangent = other.tangent;
	}

public:
	Vector3 m_position;
	Vector3 m_normal;
	Vector4 m_tangent;

	Rgba m_color;
	Vector2 m_UVs;

	static const vertex_attribute_t s_attributes[];
	static const VertexLayout s_layout;
};

