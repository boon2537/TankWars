#pragma once

#include <string>
#include "Engine/Math/Vector2.hpp"

class Vector3
{
public:
	// Construction/Destruction
	~Vector3() {}											// destructor: do nothing (for speed)
	Vector3() {}											// default constructor: do nothing (for speed)
	Vector3( const Vector3& copyFrom );						// copy constructor (from another vec3)
	explicit Vector3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y, z)
	explicit Vector3(float initialValues);
	explicit Vector3(const Vector2& vec2, float z);

	// Operators
	const Vector3 operator+( const Vector3& vecToAdd ) const;		// vec3 + vec3
	const Vector3 operator-( const Vector3& vecToSubtract ) const;	// vec3 - vec3
	const Vector3 operator*( float uniformScale ) const;			// vec3 * float
	const Vector3 operator/( float inverseScale ) const;			// vec3 / float
	void operator+=( const Vector3& vecToAdd );						// vec3 += vec3
	void operator-=( const Vector3& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );					// vec3 *= float
	void operator/=( const float uniformDivisor );					// vec3 /= float
	void operator=( const Vector3& copyFrom );						// vec3 = vec3
	bool operator==( const Vector3& compare ) const;				// vec3 == vec3
	bool operator!=( const Vector3& compare ) const;				// vec3 != vec3

	bool operator<(const Vector3& compare) const;
	bool operator>=(const Vector3& compare) const; // if !(a < b) => a >= b
	bool operator>(const Vector3& compare) const;
	bool operator<=(const Vector3& compare) const; // if !(a > b) => a <= b

	float GetLength() const;
	float GetLengthSquared() const; // faster than GetLength() since it skips the sqrtf()
	float NormalizeAndGetLength(); // set my new length to 1.0f; keep my direction
	Vector3 GetNormalized() const; // return a new vector, which is a normalized copy of me

	void SetFromText(const char* text, char delimiter = ',');
	std::string ToString();

public:
	float x;
	float y;
	float z;

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 forward;
	static const Vector3 right;
	static const Vector3 up;
};

const Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd );
const Vector3 Slerp( const Vector3& a, const Vector3& b, float t );
const Vector3 SlerpUnit( const Vector3& a, const Vector3& b, float t );

Vector3 Average(const Vector3& a, const Vector3& b);
// min is component max;
Vector3 Min(const Vector3& a, const Vector3& b);
Vector3 Max(const Vector3& a, const Vector3& b);