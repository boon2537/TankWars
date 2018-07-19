#include "Engine/Renderer/Sampler.hpp"

Sampler::Sampler() 
	: m_samplerHandle(NULL)
{}

Sampler::~Sampler()
{
	Destroy();
}

bool Sampler::Create()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL) 
	{
		glGenSamplers( 1, &m_samplerHandle ); 
		if (m_samplerHandle == NULL) 
		{
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_REPEAT );  

	// filtering; 
// 	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
// 	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	// filtering; (trilinear) - really - if you want mips with nearest, siwtch the GL_LINEAR_* to GL_NEAREST_*
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );         // Default: GL_LINEAR
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );						// Default: GL_LINEAR

	// Limit where on the LOD I can actually fetch (if you want to control it through the sampler)
	// defaults to -1000.0f to 1000.0f
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MIN_LOD, -1000.0f ); 
	glSamplerParameterf( m_samplerHandle, GL_TEXTURE_MAX_LOD, 1000.0f ); 

	return true; 
}

bool Sampler::CrateShadowSampler()
{
	// create the sampler handle if needed; 
	if (m_samplerHandle == NULL) 
	{
		glGenSamplers( 1, &m_samplerHandle ); 
		if (m_samplerHandle == NULL) 
		{
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );  
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );  

	// filtering; 
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	float test[4];
	test[0] = 1.f;
	test[1] = 1.f;
	test[2] = 1.f;
	test[3] = 1.f;

	glSamplerParameterfv( m_samplerHandle, GL_TEXTURE_BORDER_COLOR, test );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
	glSamplerParameteri( m_samplerHandle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
	return true; 
}

void Sampler::Destroy()
{
	if (m_samplerHandle != NULL) 
	{
		glDeleteSamplers( 1, &m_samplerHandle ); 
		m_samplerHandle = NULL; 
	}
}

GLuint Sampler::GetHandle()
{
	return m_samplerHandle;
}
