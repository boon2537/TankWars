//-----------------------------------------------------------------------------------------------
// Texture.cpp
//

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/ThirdParty/stb_image.h"
#include "Engine/Renderer/glFunctions.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"

//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//

Texture::Texture()
{

}

Texture::Texture(const std::string& imageFilePath)
	: m_handle(0)
	, m_dimensions(0, 0)
{
	// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	Image* image = new Image(imageFilePath);
	CreateFromImage(image);

	delete image;
}

Texture::Texture(Image* image)
{
	CreateFromImage(image);
}

void Texture::CreateFromImage(Image* image)
{
	PopulateFromData(image->GetData(), image->GetDimensions(), 4);
}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData(unsigned char* imageData, const IntVector2& texelSize, int numComponents)
{
	m_dimensions = texelSize;

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures(1, &m_handle);

	GLenum bufferFormat = GL_RGBA8; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (numComponents == 3)
		bufferFormat = GL_RGB8;

	GLenum internal_format; 
	GLenum channels; 
	GLenum pixel_layout; 
	Renderer::TextureGetInternalFormat(&internal_format, &channels, &pixel_layout, bufferFormat); 

	// Copy the texture - first, get use to be using texture unit 0 for this;
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_handle );  

	GL_CHECK_ERROR();
	int mipCount = Texture::CalculateMipCount(m_dimensions.x, m_dimensions.y);

	if (mipCount == 0)
		mipCount = 1;

	// create the gpu buffer
	// note: only this is needed for render targets
	glTexStorage2D( GL_TEXTURE_2D,
		mipCount,        // number of levels (mip-layers)
		internal_format, // how is the memory stored on the GPU
		m_dimensions.x, m_dimensions.y ); // dimensions
	GL_CHECK_ERROR();
	// copies cpu memory to the gpu - needed for texture resources
	glTexSubImage2D( GL_TEXTURE_2D,
		0,             // mip layer we're copying to
		0, 0,          // offset
		m_dimensions.x, m_dimensions.y, // dimensions
		channels,      // which channels exist in the CPU buffer
		pixel_layout,     // how are those channels stored
		imageData); // cpu buffer to copy;

	if (mipCount > 1)
		glGenerateMipmap( GL_TEXTURE_2D );

	GL_CHECK_ERROR();
}

bool Texture::CreateRenderTarget(unsigned int width, unsigned int height, TEXTURE_FORMAT format)
{
	// generate the link to this texture
	glGenTextures(1, &m_handle); 
	if (m_handle == NULL) {
		return false; 
	}

	// TODO - add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat )
	//        when more texture formats are required; 
	GLenum internal_format = GL_RGBA8; 
	GLenum channels = GL_RGBA;  
	GLenum pixel_layout = GL_UNSIGNED_BYTE; 

	if (format == TEXTURE_FORMAT_D24S8) 
	{
		internal_format = GL_DEPTH_STENCIL; 
		channels = GL_DEPTH_STENCIL; 
		pixel_layout = GL_UNSIGNED_INT_24_8; 
	}

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_handle);    // bind our texture to our current texture unit (0)

												 // Copy data into it;
	glTexImage2D(GL_TEXTURE_2D, 0, 
		internal_format, // what's the format OpenGL should use
		width, 
		height,        
		0,             // border, use 0
		channels,      // how many channels are there?
		pixel_layout,  // how is the data laid out
		nullptr);     // don't need to pass it initialization data 

	 // make sure it succeeded
	GL_CHECK_ERROR();

	// cleanup after myself; 
	glBindTexture(GL_TEXTURE_2D, NULL); // unset it; 

	// Save this all off
	m_dimensions.x = width;
	m_dimensions.y = height;
	m_format = format; // Save the format with the texture for sanity checking.

	return true; 
}

unsigned int Texture::GetWidth()
{
	return m_dimensions.x;
}

unsigned int Texture::GetHeight()
{
	return m_dimensions.y;
}

GLuint Texture::GetHandle()
{
	return m_handle;
}

Texture* Texture::CreateBlank(int width, int height)
{
	Texture* output = new Texture();
	output->CreateRenderTarget(width, height, TEXTURE_FORMAT::TEXTURE_FORMAT_RGBA8);
	return output;
}

Texture* Texture::CreateCompatible(Texture* texture)
{
	Texture* output = new Texture();
	output->CreateRenderTarget(texture->GetWidth(), texture->GetHeight(), TEXTURE_FORMAT::TEXTURE_FORMAT_RGBA8);
	return output;
}

void Texture::CopyTexture(Texture* source, Texture* target)
{	
	GLuint targetHandle = target->GetHandle();
	glBindTexture(GL_TEXTURE_2D, targetHandle);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, source->GetWidth(), source->GetHeight(), 0);
}

int Texture::CalculateMipCount(int width, int height)
{
	int valueToUse = MaxInt(width, height);
	int result = (int) ceil(log2(valueToUse));
	return result;
}
