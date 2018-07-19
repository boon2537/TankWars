#pragma once

#include "Engine/Math/Vector3.hpp"

class Vector4
{
public:
	// Construction/Destruction
	~Vector4() {}											// destructor: do nothing (for speed)
	Vector4() {}											// default constructor: do nothing (for speed)
	Vector4( const Vector4& copyFrom );						// copy constructor (from another vec3)
	explicit Vector4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y, z)
	explicit Vector4(const Vector3& vec3, float initialW);
																			// Operators
	const Vector4 operator+( const Vector4& vecToAdd ) const;		// vec3 + vec3
	const Vector4 operator-( const Vector4& vecToSubtract ) const;	// vec3 - vec3
	const Vector4 operator*( float uniformScale ) const;			// vec3 * float
	const Vector4 operator/( float inverseScale ) const;			// vec3 / float
	void operator+=( const Vector4& vecToAdd );						// vec3 += vec3
	void operator-=( const Vector4& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );					// vec3 *= float
	void operator/=( const float uniformDivisor );					// vec3 /= float
	void operator=( const Vector4& copyFrom );						// vec3 = vec3
	bool operator==( const Vector4& compare ) const;				// vec3 == vec3
	bool operator!=( const Vector4& compare ) const;				// vec3 != vec3

	float GetLength() const;
	float GetLengthSquared() const; // faster than GetLength() since it skips the sqrtf()
	float NormalizeAndGetLength(); // set my new length to 1.0f; keep my direction
	Vector4 GetNormalized() const; // return a new vector, which is a normalized copy of me
	Vector3 xyz() const; // return a new vec3 from me

	void SetFromText(const char* text, char delimiter = ',');

public:
	float x;
	float y;
	float z;
	float w;

	static const Vector4 zero;
};

const Vector4 Interpolate( const Vector4& start, const Vector4& end, float fractionTowardEnd );
