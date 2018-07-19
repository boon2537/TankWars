#pragma once

class IntVector3
{
public:
	// Construction/Destruction
	~IntVector3() {}											// destructor: do nothing (for speed)
	IntVector3() {}											// default constructor: do nothing (for speed)
	IntVector3( const IntVector3& copyFrom );						// copy constructor (from another vec3)
	explicit IntVector3( int initialX, int initialY, int initialZ );		// explicit constructor (from x, y, z)

																			// Operators
	const IntVector3 operator+( const IntVector3& vecToAdd ) const;		// vec3 + vec3
	const IntVector3 operator-( const IntVector3& vecToSubtract ) const;	// vec3 - vec3
	const IntVector3 operator*( float uniformScale ) const;			// vec3 * float
	const IntVector3 operator/( float inverseScale ) const;			// vec3 / float
	void operator+=( const IntVector3& vecToAdd );						// vec3 += vec3
	void operator-=( const IntVector3& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );					// vec3 *= float
	void operator/=( const float uniformDivisor );					// vec3 /= float
	void operator=( const IntVector3& copyFrom );						// vec3 = vec3
	bool operator==( const IntVector3& compare ) const;				// vec3 == vec3
	bool operator!=( const IntVector3& compare ) const;				// vec3 != vec3

public:
	int x;
	int y;
	int z;

	static const IntVector3 zero;
};