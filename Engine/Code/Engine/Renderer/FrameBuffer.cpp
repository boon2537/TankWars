#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/DevConsole.hpp"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &m_handle); //**treating a single element as an array of one**
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_handle);
	m_handle = NULL;
}

void FrameBuffer::SetColorTarget(Texture* colorTarget)
{
	m_colorTarget = colorTarget;
}

void FrameBuffer::SetDepthStencilTarget(Texture * depthTarget)
{
	m_depthStencilTarget = depthTarget;
}

bool FrameBuffer::Finalize()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle); 

	TODO("Update to support multiple target if wanna do bloom");
	// keep track of which outputs go to which attachments; 
	GLenum targets[1]; 

	// Bind a color target to an attachment point
	// and keep track of which locations to to which attachments. 
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + 0, 
		m_colorTarget->GetHandle(), 
		0); 
	// 0 to to attachment 0
	targets[0] = GL_COLOR_ATTACHMENT0 + 0; 

	// Update target bindings
	glDrawBuffers(1, targets); 

	// Bind depth if available;
	if (m_depthStencilTarget == nullptr) {
		glFramebufferTexture(GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			NULL, 
			0); 
	} else {
		glFramebufferTexture(GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			m_depthStencilTarget->GetHandle(), 
			0); 
	}

	// Error Check - recommend only doing in debug
#if defined(_DEBUG)
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) 
	{
		ConsoleErrorf("Failed to create frame-buffer:  %u", status);
		return false;
	}
#endif

	return true;
}

unsigned int FrameBuffer::GetWidth()
{
	return m_colorTarget->GetWidth();
}

unsigned int FrameBuffer::GetHeight()
{
	return m_colorTarget->GetHeight();
}
