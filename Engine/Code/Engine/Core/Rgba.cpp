#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string.h>
#include <stdio.h>

const Rgba Rgba::white = Rgba(255, 255, 255, 255);
const Rgba Rgba::black = Rgba(0, 0, 0, 255);
const Rgba Rgba::red = Rgba(255, 0, 0, 255);
const Rgba Rgba::blue = Rgba(0, 0, 255, 255);
const Rgba Rgba::green = Rgba(0, 255, 0, 255);
const Rgba Rgba::cyan = Rgba(0, 255, 255, 255);
const Rgba Rgba::magenta = Rgba(255, 0, 255, 255);
const Rgba Rgba::yellow = Rgba(255, 234, 49, 255);

Rgba::Rgba()
{

}

Rgba::Rgba(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	r = static_cast<unsigned char>(255 * normalizedRed);
	g = static_cast<unsigned char>(255 * normalizedGreen);
	b = static_cast<unsigned char>(255 * normalizedBlue);
	a = static_cast<unsigned char>(255 * normalizedAlpha);
}

void Rgba::GetAsFloats(float & out_normalizedRed, float & out_normalizedGreen, float & out_normalizedBlue, float & out_normalizedAlpha) const
{
	out_normalizedRed = (float) r / 255;
	out_normalizedGreen = (float) g / 255;
	out_normalizedBlue = (float) b / 255;
	out_normalizedAlpha = (float) a / 255;
}

void Rgba::ScaleRGB(float rgbScale)
{
	r = static_cast<unsigned char>(r * ClampFloatZeroToOne(rgbScale));
	g = static_cast<unsigned char>(g * ClampFloatZeroToOne(rgbScale));
	b = static_cast<unsigned char>(b * ClampFloatZeroToOne(rgbScale));	
}

void Rgba::ScaleAlpha(float alphaScale)
{
	a = static_cast<unsigned char>(a * ClampFloatZeroToOne(alphaScale));
}

void Rgba::SetFromText(const char * text)
{
	std::string data = std::string(text);
	size_t commaPosition = data.find(',');

	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a comma to create RGBA!" );
		return;
	}
	std::string colorText = std::string(data, 0, commaPosition);
	r = (unsigned char) atoi(colorText.c_str());

	size_t oldCommaPosition = commaPosition;
	commaPosition = data.find(',', commaPosition + 1);
	if (commaPosition == std::string::npos)
	{
		ERROR_RECOVERABLE( "WARNING: Cannot find a comma to create RGBA!" );
		return;
	}
	colorText = std::string(data, oldCommaPosition + 1, commaPosition);
	g = (unsigned char) atoi(colorText.c_str());

	oldCommaPosition = commaPosition;
	commaPosition = data.find(',', commaPosition + 1);
	//there are only two commas, ignore alpha
	if (commaPosition == std::string::npos)
	{
		colorText = std::string(data, oldCommaPosition + 1);
		b = (unsigned char) atoi(colorText.c_str());
		a = 255;
		return;
	}

	colorText = std::string(data, oldCommaPosition + 1, commaPosition);
	b = (unsigned char) atoi(colorText.c_str());

	colorText = std::string(data, commaPosition + 1);
	a = (unsigned char) atoi(colorText.c_str());
}

Rgba Rgba::GetBlendedColor(const Rgba & a, const Rgba & b)
{
	Rgba result = Rgba((a.r + b.r) / 2, (a.g + b.g) / 2, (a.b + b.b) / 2, (a.a + b.a) / 2);
	return result;
}

const Rgba Interpolate(const Rgba & start, const Rgba & end, float fractionTowardEnd)
{
	return Rgba(Interpolate(start.r, end.r, fractionTowardEnd), Interpolate(start.g, end.g, fractionTowardEnd),
		Interpolate(start.b, end.b, fractionTowardEnd), Interpolate(start.a, end.a, fractionTowardEnd));
}
