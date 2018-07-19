#pragma once

#include "Engine/Core/Rgba.hpp"
#include <string>
#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Image
{
public:
	Image() {}
	explicit Image( const std::string& imageFilePath, bool flip = true );
	explicit Image( unsigned char* data, int numComponents, IntVector2 dimensions);
	Rgba	GetTexel( int x, int y ) const; 			// (0,0) is top-left
	void	SetTexel( int x, int y, const Rgba& color );
	int		GetNumComponents() const;
	IntVector2 GetDimensions() const;
	unsigned char* GetData(int offsetX = 0, int offsetY = 0) const;
	uint GetFormat() const;

private:
	IntVector2		m_dimensions;
	std::vector<Rgba>	m_texels; // ordered left-to-right, then down, from [0]=(0,0) at top-left
	int m_numComponents = 0;
};
