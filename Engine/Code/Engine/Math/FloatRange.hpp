#pragma once

class FloatRange
{
public:
	// Construction/Destruction
	~FloatRange() {}											// destructor: do nothing (for speed)
	FloatRange() {}												// default constructor: do nothing (for speed)
	FloatRange(const FloatRange& copyFrom);						// copy constructor
	explicit FloatRange(float initialMin, float initialMax);	
	explicit FloatRange(float initialMinMax);	

	float FloatRange::GetRandomInRange() const;
	void FloatRange::SetFromText( const char* text );

public:
	float min;
	float max;
};

bool DoRangesOverlap( const FloatRange& a, const FloatRange& b );
const FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd );