#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string.h>
#include <stdio.h>

FloatRange::FloatRange(const FloatRange & copyFrom)
{
	min = copyFrom.min;
	max = copyFrom.max;
}

FloatRange::FloatRange(float initialMin, float initialMax)
{
	min = initialMin;
	max = initialMax;
}

FloatRange::FloatRange(float initialMinMax)
{
	min = max = initialMinMax;
}

float FloatRange::GetRandomInRange() const
{
	return GetRandomFloatInRange(min, max);
}

void FloatRange::SetFromText(const char * text)
{
	std::string data = std::string(text);
	size_t tildePosition = data.find('~');

	if (tildePosition == std::string::npos)
	{
		min = max = (float) atof(data.c_str());
		return;
	}

	std::string minText = std::string(data, 0, tildePosition);
	std::string maxText = std::string(data, tildePosition + 1);

	min = (float) atof(minText.c_str());
	max = (float) atof(maxText.c_str());
}

bool DoRangesOverlap(const FloatRange & a, const FloatRange & b)
{
	return a.max > b.min || b.max > a.min;
}

const FloatRange Interpolate(const FloatRange & start, const FloatRange & end, float fractionTowardEnd)
{
	return FloatRange(Interpolate(start.min, end.min, fractionTowardEnd), Interpolate(start.max, end.max, fractionTowardEnd));
}
