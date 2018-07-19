#pragma once

#include "Engine/Renderer/glFunctions.hpp"

class Sampler
{
public:
	Sampler();
	~Sampler();

	bool Create();
	bool CrateShadowSampler();
	void Destroy();
	GLuint GetHandle();

private:
	GLuint m_samplerHandle;
};

