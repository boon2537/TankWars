#include "Engine/Math/Disc2.hpp"
#include <math.h>

Disc2::Disc2(const Disc2 & copyFrom)
{
	center = copyFrom.center;
	radius = copyFrom.radius;
}

Disc2::Disc2(float initialX, float initialY, float initialRadius)
{
	center = Vector2(initialX, initialY);
	radius = initialRadius;
}

Disc2::Disc2(const Vector2 & initialCenter, float initialRadius)
{
	center = initialCenter;
	radius = initialRadius;
}

void Disc2::StretchToIncludePoint(float x, float y)
{
	if (!IsPointInside(x, y))
	{
		float dx = x - center.x;
		float dy = y - center.y;
		radius = sqrtf((dx * dx) + (dy * dy));
	}
}

void Disc2::StretchToIncludePoint(const Vector2 & point)
{
	if (!IsPointInside(point.x, point.y))
	{
		float dx = point.x - center.x;
		float dy = point.y - center.y;
		radius = sqrtf((dx * dx) + (dy * dy));
	}
}

void Disc2::AddPaddingToRadius(float paddingRadius)
{
	radius += paddingRadius;
}

void Disc2::Translate(const Vector2 & translation)
{
	center += translation;
}

void Disc2::Translate(float translationX, float translationY)
{
	center.x += translationX;
	center.y += translationY;
}

bool Disc2::IsPointInside(float x, float y) const
{
	float dx = x - center.x;
	float dy = y - center.y;
	return (dx * dx) + (dy * dy) < (radius * radius);
}

bool Disc2::IsPointInside(const Vector2 & point) const
{
	float dx = point.x - center.x;
	float dy = point.y - center.y;
	return (dx * dx) + (dy * dy) < (radius * radius);
}

void Disc2::operator+=(const Vector2 & translation)
{
	center += translation;
}

void Disc2::operator-=(const Vector2 & antiTranslation)
{
	center -= antiTranslation;
}

Disc2 Disc2::operator+(const Vector2 & translation) const
{
	return Disc2(center + translation, radius);
}

Disc2 Disc2::operator-(const Vector2 & antiTranslation) const
{
	return Disc2(center - antiTranslation, radius);
}

const Disc2 Interpolate(const Disc2 & start, const Disc2 & end, float fractionTowardEnd)
{
	return Disc2(Interpolate(start.center, end.center, fractionTowardEnd), Interpolate(start.radius, end.radius, fractionTowardEnd));
}
