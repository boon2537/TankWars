#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const AABB2 AABB2::ZERO_TO_ONE = AABB2(0, 0, 1, 1);

AABB2::AABB2(const AABB2 & copy)
{
	mins = copy.mins;
	maxs = copy.maxs;
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	mins = Vector2(minX, minY);
	maxs = Vector2(maxX, maxY);
}

AABB2::AABB2(const Vector2 & mins, const Vector2 & maxs)
{
	this->mins = mins;
	this->maxs = maxs;
}

AABB2::AABB2(const Vector2 & center, float radiusX, float radiusY)
{
	mins = Vector2(center.x - radiusX, center.y - radiusY);
	maxs = Vector2(center.x + radiusX, center.y + radiusY);
}

void AABB2::StretchToIncludePoint(float x, float y)
{
	if (x < mins.x)
		mins.x = x;
	else if (x > maxs.x)
		maxs.x = x;
	if (y < mins.y)
		mins.y = y;
	else if (y > maxs.y)
		maxs.y = y;
}

void AABB2::StretchToIncludePoint(const Vector2 & point)
{
	if (point.x < mins.x)
		mins.x = point.x;
	else if (point.x > maxs.x)
		maxs.x = point.x;
	if (point.y < mins.y)
		mins.y = point.y;
	else if (point.y > maxs.y)
		maxs.y = point.y;
}

void AABB2::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius)
{
	mins = Vector2(mins.x - xPaddingRadius, mins.y - yPaddingRadius);
	maxs = Vector2(maxs.x + xPaddingRadius, maxs.y + yPaddingRadius);
}

void AABB2::Translate(const Vector2 & translation)
{
	mins += translation;
	maxs += translation;
}

void AABB2::Translate(float translationX, float translationY)
{
	mins.x += translationX;
	mins.y += translationY;
	maxs.x += translationX;
	maxs.y += translationY;
}

bool AABB2::IsPointInside(float x, float y) const
{
	return x >= mins.x && x <= maxs.x && y >= mins.y && y <= maxs.y;
}

bool AABB2::IsPointInside(const Vector2 & point) const
{
	return point.x >= mins.x && point.x <= maxs.x && point.y >= mins.y && point.y <= maxs.y;
}

Vector2 AABB2::GetDimensions() const
{
	return Vector2(maxs.x - mins.x, maxs.y - mins.y);
}

Vector2 AABB2::GetCenter() const
{
	return Vector2((maxs.x + mins.x) / 2, (maxs.y + mins.y) / 2);
}

void AABB2::operator+=(const Vector2 & translation)
{
	Translate(translation);
}

void AABB2::operator-=(const Vector2 & antiTranslation)
{
	Translate(-1 * antiTranslation);
}

AABB2 AABB2::operator+(const Vector2 & translation) const
{
	return AABB2(mins + translation, maxs + translation);
}

AABB2 AABB2::operator-(const Vector2 & antiTranslation) const
{
	return AABB2(mins - antiTranslation, maxs - antiTranslation);
}

void AABB2::SetFromText(const char * text)
{
	Strings data = Split(text, ',');
	if (data.size() != 4)
		ERROR_RECOVERABLE("WARNING: AABB2 Text is formatted wrong!");

	mins = Vector2((float) atof(data[0].c_str()), (float) atof(data[1].c_str()));
	maxs = Vector2((float) atof(data[2].c_str()), (float) atof(data[3].c_str()));
}

const AABB2 Interpolate(const AABB2 & start, const AABB2 & end, float fractionTowardEnd)
{
	return AABB2(Interpolate(start.mins, end.mins, fractionTowardEnd), Interpolate(start.maxs, end.maxs, fractionTowardEnd));
}
