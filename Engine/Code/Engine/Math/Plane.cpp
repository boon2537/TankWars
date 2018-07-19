#include "Engine/Math/Plane.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Plane::Plane(const Vector3& normal, const Vector3& pos)
	: normal(normal)
{
	distance = DotProduct(normal, pos);
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
// ccw is a normal point torward us
	Vector3 e0 = b - a;
	Vector3 e1 = c - a;

	normal = CrossProduct(e1, e0);
	ASSERT_RECOVERABLE(!IsMostlyEqual(normal.GetLength(), 0.f), "normal should not be zero, invalid plane");

	normal.NormalizeAndGetLength();
	distance = DotProduct(normal, a);
}

float Plane::GetDistance(const Vector3& pos) const
{
	float dist = DotProduct(pos, normal);
	return dist;
}

bool Plane::IsInFront(const Vector3& pos) const
{
	return GetDistance(pos) > 0.f;
}

void Plane::FlipNormal()
{
	normal = normal * -1;
	distance = distance * -1;
}

bool DoLineIntersectsPlane(const Vector3 & start, const Vector3& end, const Plane& p)
{
	float d0 = p.GetDistance(start); 
	float d1 = p.GetDistance(end); 

	return (d0 * d1) <= 0.0f; 
}
