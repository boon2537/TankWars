#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/glFunctions.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RendererTypes.hpp"

static Image MassageImageForTexture(const Image& src)
{
	//assume we have only GL_RGB8 or GL_RGBA8 format
	Image copy = Image(src.GetData(), src.GetNumComponents(), src.GetDimensions());

	// ImageFlipY( &copy ); // do not flip for CubeMaps - they use a different coordinate system; 
	return copy; 
}

//------------------------------------------------------------------------
static void BindImageToSide(eTextureCubeSide side, const Image& img, uint size, uint ox, uint oy, GLenum channels, GLenum pixel_layout) 
{
	void const *ptr = img.GetData(ox, oy);
	glTexSubImage2D(Renderer::ToGLCubeSide(side),
		0,          // mip_level
		0, 0,       // offset
		size, size, 
		channels, 
		pixel_layout, 
		ptr); 

	GL_CHECK_ERROR(); 
}

//------------------------------------------------------------------------
static void FlipAndBindImage(eTextureCubeSide side, Image const &img, GLenum channels, GLenum pixel_layout) 
{
	Image copy = MassageImageForTexture(img); 
	BindImageToSide(side, copy, copy.GetDimensions().x, 0, 0, channels, pixel_layout); 
}

TextureCube::~TextureCube()
{
	CleanUp();
}

TextureCube::TextureCube()
{
	m_size = 0U; 
	m_handle = 0U; 
}

void TextureCube::CleanUp()
{
	if (m_handle != NULL) 
	{
		glDeleteTextures(1, &m_handle);
		m_handle = NULL; 
	}

	m_size = 0; 
}

bool TextureCube::MakeFromImages(const Image* images)
{
	if (m_handle == NULL) 
	{
		glGenTextures( 1, &m_handle ); 
	}

	m_size = images[0].GetDimensions().x; 
	m_format = images[0].GetFormat(); 

	GLenum internal_format; 
	GLenum channels; 
	GLenum pixel_layout; 
	Renderer::TextureGetInternalFormat(&internal_format, &channels, &pixel_layout, m_format); 

	// bind it; 
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle); 

	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internal_format, m_size, m_size); 
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); 

	// bind the image to the side; 
	FlipAndBindImage(TEXCUBE_RIGHT,  images[0], channels, pixel_layout); 
	FlipAndBindImage(TEXCUBE_LEFT,   images[1], channels, pixel_layout); 
	FlipAndBindImage(TEXCUBE_TOP,    images[2], channels, pixel_layout); 
	FlipAndBindImage(TEXCUBE_BOTTOM, images[3], channels, pixel_layout); 
	FlipAndBindImage(TEXCUBE_FRONT,  images[4], channels, pixel_layout); 
	FlipAndBindImage(TEXCUBE_BACK,   images[5], channels, pixel_layout); 

	return true;
}

bool TextureCube::MakeFromImages(const Image& image)
{
	uint width = image.GetDimensions().x; 
	uint height = image.GetDimensions().y;
	uint size = width / 4; 

	// make sure it is the type I think it is; 
	ASSERT_RECOVERABLE(height == (size * 3U), "not the type expected");

	if (m_handle == NULL) 
	{
		glGenTextures( 1, &m_handle );
	}

	m_size = size; 
	m_format = image.GetFormat(); 
	Image copy = MassageImageForTexture(image);  

	GLenum internal_format; 
	GLenum channels; 
	GLenum pixel_layout; 
	Renderer::TextureGetInternalFormat(&internal_format, &channels, &pixel_layout, m_format); 
	GL_CHECK_ERROR(); 

	// bind it; 
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle); 
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internal_format, m_size, m_size); 
	GL_CHECK_ERROR(); 

	glPixelStorei(GL_UNPACK_ROW_LENGTH, copy.GetDimensions().x); 
	GL_CHECK_ERROR(); 

	// bind the image to the side; 
	BindImageToSide(TEXCUBE_RIGHT,  copy, m_size, m_size * 2, m_size * 1, channels, pixel_layout); 
	BindImageToSide(TEXCUBE_LEFT,   copy, m_size, m_size * 0, m_size * 1, channels, pixel_layout); 
	BindImageToSide(TEXCUBE_TOP,    copy, m_size, m_size * 1, m_size * 0, channels, pixel_layout); 
	BindImageToSide(TEXCUBE_BOTTOM, copy, m_size, m_size * 1, m_size * 2, channels, pixel_layout); 
	BindImageToSide(TEXCUBE_FRONT,  copy, m_size, m_size * 1, m_size * 1, channels, pixel_layout); 
	BindImageToSide(TEXCUBE_BACK,   copy, m_size, m_size * 3, m_size * 1, channels, pixel_layout); 

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); 

	return true; 
}

bool TextureCube::MakeFromImages(const char* fileName)
{
	Image img = Image(fileName, false); 
	return MakeFromImages(img); 
}
