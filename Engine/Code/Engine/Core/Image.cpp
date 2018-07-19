#include "Image.hpp"
#include "Engine/ThirdParty/stb_image.h"
#include "Engine/Renderer/glFunctions.hpp"
#include <vector>

Image::Image(const std::string & imageFilePath, bool flip)
{
	//flip y
	if (flip)
	{
		stbi_set_flip_vertically_on_load(1);
	}

	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)
	unsigned char* imageData = stbi_load(imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested);
	m_numComponents = numComponents;
	unsigned char* beginData = imageData;

	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			m_texels.push_back(Rgba::magenta);

			unsigned char r = *imageData;
			imageData++;
			unsigned char g = *imageData;
			imageData++;
			unsigned char b = *imageData;
			imageData++;
			unsigned char a = 255;
			if (m_numComponents == 4)
			{
				a = *imageData;
				imageData++;
			}
			
			SetTexel(x, y, Rgba(r, g, b, a));
		}
	}

	stbi_image_free(beginData);

	//un-flip y
	stbi_set_flip_vertically_on_load(0);
}

Image::Image(unsigned char* data, int numComponents, IntVector2 dimensions)
{
	m_dimensions = dimensions;
	m_numComponents = numComponents;
	unsigned char* imageData = data;

	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			m_texels.push_back(Rgba::magenta);

			unsigned char r = *imageData;
			imageData++;
			unsigned char g = *imageData;
			imageData++;
			unsigned char b = *imageData;
			imageData++;
			unsigned char a = 255;
			if (m_numComponents == 4)
			{
				a = *imageData;
				imageData++;
			}

			SetTexel(x, y, Rgba(r, g, b, a));
		}
	}
}

Rgba Image::GetTexel(int x, int y) const
{
	int index = x + (m_dimensions.x * y);
	return m_texels[index];
}

void Image::SetTexel(int x, int y, const Rgba & color)
{
	int index = x + (m_dimensions.x * y);
	m_texels[index] = color;
}

int Image::GetNumComponents() const
{
	return m_numComponents;
}

IntVector2 Image::GetDimensions() const
{
	return m_dimensions;
}

unsigned char* Image::GetData(int offsetX, int offsetY) const
{
	int index = offsetX + (m_dimensions.x * offsetY);
	return (unsigned char*) (m_texels.data() + index);
}

uint Image::GetFormat() const
{
	//we only support 3 (RGB) or 4 (RGBA)
	if (m_numComponents == 3)	return GL_RGB8;
	if (m_numComponents == 4)	return GL_RGBA8;

	ASSERT_RECOVERABLE(false, "Format not supported");
	return NULL;
}
