#pragma once

#include "Engine/Math/Vector3.hpp"

struct Ray3
{
public:
	~Ray3() {};
	Ray3() {};
	Ray3(const Vector3& start, const Vector3 direction)
		: start(start), direction(direction) {}

	Vector3 Evaluate(float t) const
	{
		return start + direction * t; 
	}

public:
	Vector3 start; 
	Vector3 direction; 
};
