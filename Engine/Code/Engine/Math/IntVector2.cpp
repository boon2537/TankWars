#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h> 
#include <string.h>
#include <stdio.h>

//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2(const IntVector2& copy)
	: x(0)
	, y(0)
{
	x = copy.x;
	y = copy.y;
}


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2(int initialX, int initialY)
	: x(0)
	, y(0)
{
	x = initialX;
	y = initialY;
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator + (const IntVector2& vecToAdd) const
{
	return IntVector2(x + vecToAdd.x, y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator-(const IntVector2& vecToSubtract) const
{
	return IntVector2(x - vecToSubtract.x, y - vecToSubtract.y);
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator*(float uniformScale) const
{
	return IntVector2(static_cast<int>(x * uniformScale), static_cast<int>(y * uniformScale));
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator/(float inverseScale) const
{
	return IntVector2(static_cast<int>(x / inverseScale), static_cast<int>(y / inverseScale));
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator+=(const IntVector2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator-=(const IntVector2& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator*=(const float uniformScale)
{
	x = static_cast<int>(x * uniformScale);
	y = static_cast<int>(y * uniformScale);
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator/=(const float uniformDivisor)
{
	x = static_cast<int>(x / uniformDivisor);
	y = static_cast<int>(y / uniformDivisor);
}


//-----------------------------------------------------------------------------------------------
void IntVector2::operator=(const IntVector2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const IntVector2 operator*(float uniformScale, const IntVector2& vecToScale)
{
	return IntVector2(static_cast<int>(vecToScale.x * uniformScale), static_cast<int>(vecToScale.y * uniformScale));
}

//-----------------------------------------------------------------------------------------------
bool IntVector2::operator==(const IntVector2& compare) const
{
	return x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool IntVector2::operator!=(const IntVector2& compare) const
{
	return x != compare.x || y != compare.y;
}

void IntVector2::SetFromText(const char * text)
{
	std::string data = std::string(text);
	size_t commaPosition = data.find(',');

	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a comma to create IntVector2!" );
		return;
	}

	std::string xText = std::string(data, 0, commaPosition);
	std::string yText = std::string(data, commaPosition + 1);

	x = atoi(xText.c_str());
	y = atoi(yText.c_str());
}

const IntVector2 Interpolate(const IntVector2 & start, const IntVector2 & end, float fractionTowardEnd)
{
	return IntVector2(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd));
}
