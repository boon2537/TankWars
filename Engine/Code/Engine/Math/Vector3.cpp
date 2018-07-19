#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <math.h> 
#include <string.h>
#include <stdio.h>

const Vector3 Vector3::zero = Vector3(0, 0, 0);
const Vector3 Vector3::one = Vector3(1, 1, 1);
const Vector3 Vector3::forward = Vector3(1, 0, 0);
const Vector3 Vector3::right = Vector3(0, 0, 1);
const Vector3 Vector3::up = Vector3(0, 1, 0);

Vector3::Vector3(const Vector3 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

Vector3::Vector3(float initialX, float initialY, float initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vector3::Vector3(float initialValues)
{
	x = initialValues;
	y = initialValues;
	z = initialValues;
}

Vector3::Vector3(const Vector2& vec2, float z)
{
	x = vec2.x;
	y = vec2.y;
	this->z = z;
}

const Vector3 Vector3::operator+(const Vector3 & vecToAdd) const
{
	return Vector3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

const Vector3 Vector3::operator-(const Vector3 & vecToSubtract) const
{
	return Vector3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

const Vector3 Vector3::operator*(float uniformScale) const
{
	return Vector3(x * uniformScale, y * uniformScale, z * uniformScale);
}

const Vector3 Vector3::operator/(float inverseScale) const
{
	return Vector3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void Vector3::operator+=(const Vector3 & vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void Vector3::operator-=(const Vector3 & vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void Vector3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void Vector3::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}

void Vector3::operator=(const Vector3 & copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

bool Vector3::operator==(const Vector3 & compare) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}

bool Vector3::operator!=(const Vector3 & compare) const
{
	return x != compare.x || y != compare.y || z != compare.z;
}

bool Vector3::operator<(const Vector3& compare) const
{
	TODO("Below ops might be faulty, Actually test this");
	return x < compare.x && y < compare.y && z < compare.z;
}

bool Vector3::operator>=(const Vector3& compare) const
{
	return !(*this < compare);
}

bool Vector3::operator>(const Vector3& compare) const
{
	return x > compare.x && y > compare.y && z > compare.z;
}

bool Vector3::operator<=(const Vector3& compare) const
{
	return !(*this > compare);
}

float Vector3::GetLength() const
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

float Vector3::GetLengthSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

float Vector3::NormalizeAndGetLength()
{
	float length = GetLength();
	if (length != 0)
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
		z *= scale;
	}
	return length;
}

Vector3 Vector3::GetNormalized() const
{
	float length = GetLength();
	if (length != 0)
	{
		float scale = 1.f / length;
		return Vector3(x * scale, y * scale, z * scale);
	}
	return Vector3::zero;
}

void Vector3::SetFromText(const char* text, char delimiter)
{
	std::string data = std::string(text);
	size_t commaPosition = data.find(delimiter);

	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a delimiter to create Vector3!" );
		return;
	}
	std::string component = std::string(data, 0, commaPosition);
	x = (float) atof(component.c_str());

	size_t oldCommaPosition = commaPosition;
	commaPosition = data.find(delimiter, commaPosition + 1);
	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a delimiter to create Vector3!" );
		return;
	}
	component = std::string(data, oldCommaPosition + 1, commaPosition);
	y = (float) atof(component.c_str());

	component = std::string(data, commaPosition + 1);
	z = (float) atof(component.c_str());
}

std::string Vector3::ToString()
{
	std::string output = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
	return output;
}

const Vector3 Interpolate(const Vector3 & start, const Vector3 & end, float fractionTowardEnd)
{
	return Vector3(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd), Interpolate(start.z, end.z, fractionTowardEnd));
}

const Vector3 Slerp(const Vector3& a, const Vector3& b, float t)
{
	float al = a.GetLength();
	float bl = b.GetLength();

	float len = Interpolate(al, bl, t);
	Vector3 u = SlerpUnit( a / al, b / bl, t ); 
	return u * len;
}

const Vector3 SlerpUnit(const Vector3 & a, const Vector3 & b, float t)
{
	float cosangle = ClampFloat(DotProduct(a, b), -1.0f, 1.0f);
	float angle = acosf(cosangle);
	if (angle < MathUtils::EPSILON) 
	{
		return Interpolate(a, b, t);
	} 
	else 
	{
		float pos_num = sinf(t * angle);
		float neg_num = sinf((1.0f - t) * angle);
		float den = sinf(angle);

		return (a * (neg_num / den))  + (b * (pos_num / den));
	}
}

Vector3 Average(const Vector3& a, const Vector3& b)
{
	return (a + b) * 0.5f;
}

Vector3 Min(const Vector3& a, const Vector3& b)
{
	return Vector3(MinFloat(a.x, b.x), MinFloat(a.y, b.y), MinFloat(a.z, b.z)); 
}

Vector3 Max(const Vector3& a, const Vector3& b)
{
	return Vector3(MaxFloat(a.x, b.x), MaxFloat(a.y, b.y), MaxFloat(a.z, b.z)); 
}
