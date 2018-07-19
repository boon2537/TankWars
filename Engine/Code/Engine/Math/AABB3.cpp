#include "Engine/Math/AABB3.hpp"

AABB3::AABB3()
	: min(INFINITY),
	max(-INFINITY)
{}

AABB3::AABB3(const AABB3& copy)
{
	min = copy.min;
	max = copy.max;
}

AABB3::AABB3(const Vector3& mins, const Vector3& maxs)
	: min(mins),
	max(maxs)
{}

void AABB3::GrowToContain(const Vector3& point)
{
	min = Min(point, min); 
	max = Max(point, max); 
}

void AABB3::Invalidate()
{
	min = Vector3(INFINITY);
	max = Vector3(-INFINITY);
}

bool AABB3::IsValid() const
{
	return max.x >= min.x;
}

bool AABB3::IsContained(const Vector3& pos)
{
	return (pos.x >= min.x && pos.x <= max.x) &&
		(pos.y >= min.y && pos.y <= max.y) &&
		(pos.z >= min.z && pos.z <= max.z);
}

Vector3 AABB3::GetCenter() const
{
	return Vector3((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + min.z) / 2);
}

Vector3 AABB3::GetDimensions() const
{
	return Vector3(max.x - min.x, max.y - min.y, max.z - min.z);
}

AABB3 AABB3::MakeFromDimensions(const Vector3 & center, const Vector3 & dimensions)
{
	float x_min = (-1 * (dimensions.x / 2)) + center.x;
	float x_max = (dimensions.x / 2) + center.x;
	float y_min = (-1 * (dimensions.y / 2)) + center.y;
	float y_max = (dimensions.y / 2) + center.y;
	float z_min = (-1 * (dimensions.z / 2)) + center.z;
	float z_max = (dimensions.z / 2) + center.z;

	return AABB3(Vector3(x_min, y_min, z_min), Vector3(x_max, y_max, z_max));
}
