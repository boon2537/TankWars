#pragma once

#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
class IntVector2
{
public:
	// Construction/Destruction
	~IntVector2() {}											// destructor: do nothing (for speed)
	IntVector2() {}											// default constructor: do nothing (for speed)
	IntVector2(const IntVector2& copyFrom);						// copy constructor (from another vec2)
	explicit IntVector2(int initialX, int initialY);		// explicit constructor (from x, y)

															// Operators
	const IntVector2 operator+(const IntVector2& vecToAdd) const;		// vec2 + vec2
	const IntVector2 operator-(const IntVector2& vecToSubtract) const;	// vec2 - vec2
	const IntVector2 operator*(float uniformScale) const;			// vec2 * float
	const IntVector2 operator/(float inverseScale) const;			// vec2 / float
	void operator+=(const IntVector2& vecToAdd);						// vec2 += vec2
	void operator-=(const IntVector2& vecToSubtract);				// vec2 -= vec2
	void operator*=(const float uniformScale);					// vec2 *= float
	void operator/=(const float uniformDivisor);					// vec2 /= float
	void operator=(const IntVector2& copyFrom);						// vec2 = vec2
	bool operator==(const IntVector2& compare) const;				// vec2 == vec2
	bool operator!=(const IntVector2& compare) const;				// vec2 != vec2
	friend const IntVector2 operator*(float uniformScale, const IntVector2& vecToScale);	// float * vec2

	void IntVector2::SetFromText( const char* text );

public:
	int x;
	int y;
};

const IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTowardEnd );
