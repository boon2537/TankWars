#pragma once

#include "Engine/Math/Vector3.hpp"

class Plane
{
public:
	~Plane() {}
	explicit Plane(const Vector3& normal, const Vector3& pos);
	explicit Plane(const Vector3& a, const Vector3& b, const Vector3& c);

	float GetDistance(const Vector3& pos) const;
	bool IsInFront(const Vector3& pos) const;
	void FlipNormal();

public:
	Vector3 normal;
	float distance; 
};

bool DoLineIntersectsPlane(const Vector3& start, const Vector3& end, const Plane& p);
