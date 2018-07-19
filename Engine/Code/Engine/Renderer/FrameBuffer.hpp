#pragma once

#include "Engine/Renderer/glFunctions.hpp"

class Texture;

class FrameBuffer
{
public:
	FrameBuffer(); 
	~FrameBuffer();

	// should just update members
	// finalize does the actual binding
	void SetColorTarget(Texture*color_target); 
	void SetDepthStencilTarget(Texture*depth_target); 

	// setups the the GL frame-buffer - called before us. 
	// TODO: Make sure this only does work if the targets
	// have changed.
	bool Finalize(); 

	unsigned int GetWidth();
	unsigned int GetHeight();

public:
	GLuint m_handle; 
	Texture* m_colorTarget; 
	Texture* m_depthStencilTarget; 
};
