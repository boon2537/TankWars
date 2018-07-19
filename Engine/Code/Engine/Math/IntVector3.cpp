#include "Engine/Math/IntVector3.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h> 
#include <string.h>
#include <stdio.h>

const IntVector3 IntVector3::zero = IntVector3(0, 0, 0);

IntVector3::IntVector3(const IntVector3 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

IntVector3::IntVector3(int initialX, int initialY, int initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

const IntVector3 IntVector3::operator+(const IntVector3 & vecToAdd) const
{
	return IntVector3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

const IntVector3 IntVector3::operator-(const IntVector3 & vecToSubtract) const
{
	return IntVector3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

const IntVector3 IntVector3::operator*(float uniformScale) const
{
	return IntVector3(static_cast<int>(x * uniformScale), static_cast<int>(y * uniformScale), static_cast<int>(z * uniformScale));
}

const IntVector3 IntVector3::operator/(float inverseScale) const
{
	return IntVector3(static_cast<int>(x / inverseScale), static_cast<int>(y / inverseScale), static_cast<int>(z / inverseScale));
}

void IntVector3::operator+=(const IntVector3 & vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void IntVector3::operator-=(const IntVector3 & vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void IntVector3::operator*=(const float uniformScale)
{
	x = static_cast<int>(x * uniformScale);
	y = static_cast<int>(y * uniformScale);
	z = static_cast<int>(z * uniformScale);
}

void IntVector3::operator/=(const float uniformDivisor)
{
	x = static_cast<int>(x / uniformDivisor);
	y = static_cast<int>(y / uniformDivisor);
	z = static_cast<int>(z / uniformDivisor);
}

void IntVector3::operator=(const IntVector3 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

bool IntVector3::operator==(const IntVector3 & compare) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}

bool IntVector3::operator!=(const IntVector3 & compare) const
{
	return x != compare.x || y != compare.y || z != compare.z;
}
