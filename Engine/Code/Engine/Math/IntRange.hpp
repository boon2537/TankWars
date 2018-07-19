#pragma once

class IntRange
{
public:
	// Construction/Destruction
	~IntRange() {}											// destructor: do nothing (for speed)
	IntRange() {}												// default constructor: do nothing (for speed)
	IntRange(const IntRange& copyFrom);						// copy constructor
	explicit IntRange(int initialMin, int initialMax);	
	explicit IntRange(int initialMinMax);	

	int IntRange::GetRandomInRange() const;
	void IntRange::SetFromText( const char* text );

public:
	int min;
	int max;
};

bool DoRangesOverlap( const IntRange& a, const IntRange& b );
const IntRange Interpolate( const IntRange& start, const IntRange& end, float fractionTowardEnd );
