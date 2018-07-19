#pragma once

#include "Engine/Math/Vector3.hpp"

class AABB3
{
public:
	~AABB3() {}
	AABB3();
	AABB3(const AABB3& copy);
	explicit AABB3(const Vector3& mins, const Vector3& maxs);

	void GrowToContain(const Vector3& point);
	void Invalidate();
	bool IsValid() const;
	bool IsContained(const Vector3& pos);
	Vector3 GetCenter() const;
	Vector3 GetDimensions() const;

	static AABB3 MakeFromDimensions(const Vector3& center, const Vector3& dimensions);

public:
	Vector3 min;
	Vector3 max;
};
