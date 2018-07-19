#pragma once

#include "Engine/ThirdParty/gl/glcorearb.h"
#include "Engine/ThirdParty/gl/glext.h"
#include "Engine/ThirdParty/gl/wglext.h"

class RenderBuffer
{
public:
	RenderBuffer();   // initialize data
	~RenderBuffer();  // cleanup OpenGL resource 

	bool CopyToGPU(size_t const byte_count, void const *data); 
	bool UpdateFromCPU(size_t const byte_count, void const *data);

public:
	GLuint m_handle;       // OpenGL handle to the GPU buffer, defualt = NULL; 
	size_t m_bufferSize;  // how many bytes are in this buffer, default = 0
};