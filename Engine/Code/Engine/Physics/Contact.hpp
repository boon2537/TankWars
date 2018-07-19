#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/AABB3.hpp"

struct RayCastHit3
{
public:
	~RayCastHit3() {}
	RayCastHit3() {}
	RayCastHit3(const Vector3& position, const Vector3& normal)
		: position(position), normal(normal) { hit = true; }

public:
	bool hit = false; 
	Vector3 position;
	Vector3 normal;
};

RayCastHit3 RayCheckPlane(const Ray3& r, const Plane& p);
RayCastHit3 RayCheckAABB3(const Ray3& r, const AABB3& bounds);
RayCastHit3 RayCheckSphere(const Ray3& r, const Vector3& center, float radius);
