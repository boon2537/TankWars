#include "Engine\Physics\Contact.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"

RayCastHit3 RayCheckPlane(const Ray3& r, const Plane& p)
{
	float vel = DotProduct(r.direction, p.normal); 
	if (IsMostlyEqual(vel, 0)) 
	{
		return RayCastHit3();
	}

	float dir = DotProduct(p.normal, r.direction); 
	float dist = p.GetDistance(r.start); 
	if ((dir * dist) > 0.0f) 
	{
		return RayCastHit3(); 
	}

	float time = -dist / vel; 

	return RayCastHit3(r.Evaluate(time), p.normal); 
}

RayCastHit3 RayCheckAABB3(const Ray3& r, const AABB3& bounds)
{
	//Implementation from: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

	float tmin = (bounds.min.x - r.start.x) / r.direction.x;
	float tmax = (bounds.max.x - r.start.x) / r.direction.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (bounds.min.y - r.start.y) / r.direction.y;
	float tymax = (bounds.max.y - r.start.y) / r.direction.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return RayCastHit3();

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (bounds.min.z - r.start.z) / r.direction.z;
	float tzmax = (bounds.max.z - r.start.z) / r.direction.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return RayCastHit3();

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	float t = tmin;
	if (t < 0) 
	{
		t = tmax;
		if (t < 0) 
			return RayCastHit3();
	} 

	TODO("return proper normal later");
	return RayCastHit3(r.Evaluate(t), Vector3::zero);
}

RayCastHit3 RayCheckSphere(const Ray3& r, const Vector3& center, float radius)
{
	//Implementation from: https://www.scratchapixel.com/code.php?id=10&origin=/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes

	Vector3 L = center - r.start;
	float tca = DotProduct(L, r.direction);

	if (tca < 0) return RayCastHit3();

	float d2 = DotProduct(L, L) - tca * tca;

	if (d2 > radius * radius) return RayCastHit3();

	float thc = sqrt(radius * radius - d2);
	float t0 = tca - thc;
	float t1 = tca + thc; 

	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead
		if (t0 < 0) return RayCastHit3(); // both t0 and t1 are negative
	}

	float t = t0;

	return RayCastHit3(r.Evaluate(t), Vector3::zero);
}
