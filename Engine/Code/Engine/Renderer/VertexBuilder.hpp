#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"

struct Vertex_builder
{
	Vector3 position;
	Vector3 normal;
	Rgba color;
	Vector2 UV;
	Vector4 tangent;
};
