#pragma once

class Rgba
{
public:
	Rgba(); // Default-constructs to opaque white (255,255,255,255)
	explicit Rgba( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );

	void SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );
	void SetAsFloats( float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha=1.0f );
	void GetAsFloats( float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha ) const;
	void ScaleRGB( float rgbScale ); // Scales (and clamps) RGB components, but not A
	void ScaleAlpha( float alphaScale ); // Scales (and clamps) Alpha, RGB is untouched
	void SetFromText( const char* text );

public:
	static Rgba GetBlendedColor(const Rgba& a, const Rgba& b);

public:
	unsigned char r; // Red byte, 0-255
	unsigned char g; // Green byte, 0-255
	unsigned char b; // Blue byte, 0-255
	unsigned char a; // Alpha (opacity) byte, 0-255; 0 is transparent, 255 is opaque

	static const Rgba white;
	static const Rgba black;
	static const Rgba red;
	static const Rgba blue;
	static const Rgba green;
	static const Rgba cyan;
	static const Rgba magenta;
	static const Rgba yellow;
};

const Rgba Interpolate( const Rgba& start, const Rgba& end, float fractionTowardEnd );
