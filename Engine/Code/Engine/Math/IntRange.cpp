#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string.h>
#include <stdio.h>

IntRange::IntRange(const IntRange & copyFrom)
{
	min = copyFrom.min;
	max = copyFrom.max;
}

IntRange::IntRange(int initialMin, int initialMax)
{
	min = initialMin;
	max = initialMax;
}

IntRange::IntRange(int initialMinMax)
{
	min = max = initialMinMax;
}

int IntRange::GetRandomInRange() const
{
	return GetRandomIntInRange(min, max);
}

void IntRange::SetFromText(const char * text)
{
	std::string data = std::string(text);
	size_t tildePosition = data.find('~');

	if (tildePosition == std::string::npos)
	{
		min = max = atoi(data.c_str());
		return;
	}

	std::string minText = std::string(data, 0, tildePosition);
	std::string maxText = std::string(data, tildePosition + 1);

	min = atoi(minText.c_str());
	max = atoi(maxText.c_str());
}

bool DoRangesOverlap(const IntRange & a, const IntRange & b)
{
	return a.max > b.min || b.max > a.min;
}

const IntRange Interpolate(const IntRange & start, const IntRange & end, float fractionTowardEnd)
{
	return IntRange(Interpolate(start.min, end.min, fractionTowardEnd), Interpolate(start.max, end.max, fractionTowardEnd));
}
