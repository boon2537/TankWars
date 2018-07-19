#include "Engine/Renderer/ParticleSystem.hpp"

static ParticleSystem* g_ParticleSystem = nullptr; 

ParticleSystem::~ParticleSystem()
{
	CleanUp();
}

ParticleSystem::ParticleSystem()
{
}

void ParticleSystem::CleanUp()
{
	for each (ParticleEmitter* emitter in m_emitters)
	{
		delete emitter;
		emitter = nullptr;
	}
	m_emitters.clear();
}

void ParticleSystem::Update(Camera* cam, float deltaSeconds)
{
	for each (ParticleEmitter* emitter in m_emitters)
	{
		emitter->Update(cam, deltaSeconds);
	}

	//quick erase
	for (int i = 0; i < m_emitters.size(); ++i)
	{
		ParticleEmitter* emitter = m_emitters[i];
		if (emitter->IsReadyToCleanUp())
		{
			delete emitter;

			size_t size = m_emitters.size();
			m_emitters[i] = m_emitters[size - 1];
			m_emitters.pop_back();
			i--;
		}
	}
}

void ParticleSystem::AddEmitter(ParticleEmitter* e)
{
	m_emitters.push_back(e);
}

ParticleSystem* ParticleSystem::CreateInstance()
{
	if (g_ParticleSystem == nullptr) 
	{
		g_ParticleSystem = new ParticleSystem(); 
	}
	return g_ParticleSystem; 
}

ParticleSystem* ParticleSystem::GetInstance()
{
	return g_ParticleSystem;
}
