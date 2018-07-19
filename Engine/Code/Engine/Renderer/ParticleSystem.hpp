#pragma once

#include "Engine/Renderer/ParticleEmitter.hpp"
#include <vector>

class Camera;

class ParticleSystem
{
public:
	~ParticleSystem();
	ParticleSystem();

	void CleanUp();
	void Update(Camera* cam, float deltaSeconds);
	void AddEmitter(ParticleEmitter* e);

public:
	static ParticleSystem* CreateInstance();
	static ParticleSystem* GetInstance(); 

private:
	std::vector<ParticleEmitter*> m_emitters;
};