#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h> 
#include <string.h>
#include <stdio.h>

const Vector2 Vector2::zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::one = Vector2(1.0f, 1.0f);

//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const Vector2& copy )
	: x( 0 )
	, y( 0 )
{
	x = copy.x;
	y = copy.y;
}

//-----------------------------------------------------------------------------------------------
Vector2::Vector2( float initialX, float initialY )
	: x( 0 )
	, y( 0 )
{
	x = initialX;
	y = initialY;
}

Vector2::Vector2(const Vector3 & vec3)
{
	x = vec3.x;
	y = vec3.y;
}

//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator + ( const Vector2& vecToAdd ) const
{
	return Vector2( x + vecToAdd.x, y + vecToAdd.y );
}

//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator-( const Vector2& vecToSubtract ) const
{
	return Vector2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const Vector2 Vector2::operator*(const Vector2 & vecToMultiply) const
{
	return Vector2(x * vecToMultiply.x, y * vecToMultiply.y);
}

//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator*( float uniformScale ) const
{
	return Vector2( x * uniformScale, y * uniformScale );
}

//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator/( float inverseScale ) const
{
	return Vector2( x / inverseScale, y / inverseScale );
}

//-----------------------------------------------------------------------------------------------
void Vector2::operator+=( const Vector2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

//-----------------------------------------------------------------------------------------------
void Vector2::operator-=( const Vector2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

//-----------------------------------------------------------------------------------------------
void Vector2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

//-----------------------------------------------------------------------------------------------
void Vector2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}

//-----------------------------------------------------------------------------------------------
void Vector2::operator=( const Vector2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

//-----------------------------------------------------------------------------------------------
const Vector2 operator*( float uniformScale, const Vector2& vecToScale )
{
	return Vector2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}

//-----------------------------------------------------------------------------------------------
bool Vector2::operator==( const Vector2& compare ) const
{
	return x == compare.x && y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator!=( const Vector2& compare ) const
{
	return x != compare.x || y != compare.y;
}

float Vector2::GetLength() const
{
	return sqrtf((x * x) + (y * y));
}

float Vector2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

float Vector2::NormalizeAndGetLength()
{
	float length = GetLength();
	if (length != 0)
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
	}
	return length;
}

Vector2 Vector2::GetNormalized() const
{
	float length = GetLength();
	if (length != 0)
	{
		float scale = 1.f / length;
		return Vector2(x * scale, y * scale);
	}
	return Vector2(0, 0);
}

float Vector2::GetOrientationDegrees() const
{
	return Atan2Degress(y, x);
}

Vector2 Vector2::MakeDirectionAtDegrees(float degrees)
{
	return Vector2(CosDegrees(degrees), SinDegrees((degrees)));
}

void Vector2::SetFromText(const char * text, char delimiter)
{
	std::string data = std::string(text);
	size_t commaPosition = data.find(delimiter);
	
	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a comma to create Vector2!" );
		return;
	}

	std::string xText = std::string(data, 0, commaPosition);
	std::string yText = std::string(data, commaPosition + 1);

	x = (float) atof(xText.c_str());
	y = (float) atof(yText.c_str());
}

#pragma region StandAloneFuncitons

Vector2 Reflect(const Vector2 & v, const Vector2 & normal)
{
	float Vn = DotProduct(v, normal.GetNormalized());
	Vector2 Vn_Vector = Vn * normal.GetNormalized();
	return v - (2 * Vn_Vector);
}

const Vector2 GetProjectedVector(const Vector2 & vectorToProject, const Vector2 & projectOnto)
{
	return (DotProduct(vectorToProject, projectOnto) / (projectOnto.GetLength() * projectOnto.GetLength())) * projectOnto;
}

const Vector2 GetTransformedIntoBasis(const Vector2 & originalVector, const Vector2 & newBasisI, const Vector2 & newBasisJ)
{
	return Vector2(DotProduct(originalVector, newBasisI), DotProduct(originalVector, newBasisJ));
}

const Vector2 GetTransformedOutOfBasis(const Vector2 & vectorInBasis, const Vector2 & oldBasisI, const Vector2 & oldBasisJ)
{
	return Vector2((vectorInBasis.x * oldBasisI.x) + (vectorInBasis.y * oldBasisJ.x), (vectorInBasis.x * oldBasisI.y) + (vectorInBasis.y * oldBasisJ.y));
}

void DecomposeVectorIntoBasis(const Vector2 & originalVector, const Vector2 & newBasisI, const Vector2 & newBasisJ, Vector2 & out_vectorAlongI, Vector2 & out_vectorAlongJ)
{
	out_vectorAlongI = DotProduct(originalVector, newBasisI) * newBasisI;
	out_vectorAlongJ = DotProduct(originalVector, newBasisJ) * newBasisJ;
}

const Vector2 Interpolate(const Vector2 & start, const Vector2 & end, float fractionTowardEnd)
{
	return Vector2(Interpolate(start.x, end.x, fractionTowardEnd), Interpolate(start.y, end.y, fractionTowardEnd));
}

#pragma endregion
