//-----------------------------------------------------------------------------------------------
// Texture.hpp
//
#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/ThirdParty/gl/glcorearb.h"
#include "Engine/ThirdParty/gl/glext.h"
#include "Engine/ThirdParty/gl/wglext.h"
#include <string>
#include <map>

enum TEXTURE_FORMAT 
{
	TEXTURE_FORMAT_RGBA8, // default color format
	TEXTURE_FORMAT_D24S8, 
}; 

class Image;

//---------------------------------------------------------------------------
class Texture
{
friend class Renderer; // Textures are managed by a Renderer instance

public:
	unsigned int GetWidth();
	unsigned int GetHeight();
	GLuint GetHandle();

public:
	static Texture* CreateBlank(int width, int height);
	static Texture* CreateCompatible(Texture* texture);
	static void CopyTexture(Texture* source, Texture* target);
	static int CalculateMipCount(int width, int height);

private:
	Texture();
	Texture(const std::string& imageFilePath); // Use renderer->CreateOrGetTexture() instead!
	Texture(Image* image);

	void CreateFromImage(Image* image);
	void PopulateFromData(unsigned char* imageData, const IntVector2& texelSize, int numComponents);
	bool CreateRenderTarget(unsigned int width, unsigned int height, TEXTURE_FORMAT format = TEXTURE_FORMAT::TEXTURE_FORMAT_RGBA8);

private:
	GLuint m_handle = NULL;
	IntVector2 m_dimensions;
	TEXTURE_FORMAT m_format;
};
