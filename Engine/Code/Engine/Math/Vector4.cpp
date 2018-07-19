#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h> 
#include <string.h>
#include <stdio.h>

const Vector4 Vector4::zero = Vector4(0, 0, 0, 0);

Vector4::Vector4(const Vector4 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW)
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}

Vector4::Vector4(const Vector3& vec3, float initialW)
{
	x = vec3.x;
	y = vec3.y;
	z = vec3.z;
	w = initialW;
}

const Vector4 Vector4::operator+(const Vector4 & vecToAdd) const
{
	return Vector4(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w);
}

const Vector4 Vector4::operator-(const Vector4 & vecToSubtract) const
{
	return Vector4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);
}

const Vector4 Vector4::operator*(float uniformScale) const
{
	return Vector4(x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale);
}

const Vector4 Vector4::operator/(float inverseScale) const
{
	return Vector4(x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale);
}

void Vector4::operator+=(const Vector4 & vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}

void Vector4::operator-=(const Vector4 & vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}

void Vector4::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

void Vector4::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}

void Vector4::operator=(const Vector4 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

bool Vector4::operator==(const Vector4 & compare) const
{
	return x == compare.x && y == compare.y && z == compare.z && w == compare.w;
}

bool Vector4::operator!=(const Vector4 & compare) const
{
	return x != compare.x || y != compare.y || z != compare.z || w != compare.w;
}

float Vector4::GetLength() const
{
	return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
}

float Vector4::GetLengthSquared() const
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

float Vector4::NormalizeAndGetLength()
{
	float length = GetLength();
	if (length != 0)
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}
	return length;
}

Vector4 Vector4::GetNormalized() const
{
	float length = GetLength();
	if (length != 0)
	{
		float scale = 1.f / length;
		return Vector4(x * scale, y * scale, z * scale, w * scale);
	}
	return Vector4::zero;
}

Vector3 Vector4::xyz() const
{
	return Vector3(x, y, z);
}

void Vector4::SetFromText(const char * text, char delimiter)
{
	std::string data = std::string(text);
	size_t commaPosition = data.find(delimiter);

	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a delimiter to create Vector4!" );
		return;
	}
	std::string component = std::string(data, 0, commaPosition);
	x = (float) atof(component.c_str());

	size_t oldCommaPosition = commaPosition;
	commaPosition = data.find(delimiter, commaPosition + 1);
	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a delimiter to create Vector4!" );
		return;
	}
	component = std::string(data, oldCommaPosition + 1, commaPosition);
	y = (float) atof(component.c_str());

	oldCommaPosition = commaPosition;
	commaPosition = data.find(delimiter, commaPosition + 1);
	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a delimiter to create Vector4!" );
		return;
	}
	component = std::string(data, oldCommaPosition + 1, commaPosition);
	z = (float) atof(component.c_str());

	component = std::string(data, commaPosition + 1);
	w = (float) atof(component.c_str());
}

const Vector4 Interpolate(const Vector4 & start, const Vector4 & end, float fractionTowardEnd)
{
	return Vector4(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd), Interpolate(start.z, end.z, fractionTowardEnd), Interpolate(start.w, end.w, fractionTowardEnd));
}
