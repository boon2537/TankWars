#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Core/Time.hpp"

ParticleEmitter::~ParticleEmitter()
{
	RenderScene* scene = RenderScene::GetCurrentScene();
	scene->RemoveRenderable(m_renderable);
	m_renderable = nullptr; 
}

ParticleEmitter::ParticleEmitter()
{
	Renderer* r = Renderer::GetInstance();
	RenderScene* scene = RenderScene::GetCurrentScene();

	Shader* additiveShader = r->CreateOrGetShader("Data/Shaders/additive.xml");
	Material* mat = Material::CreateInstance("particle", additiveShader);
	mat->SetSampler(0, r->GetDefaultSampler());
	mat->SetTexture2D(0, r->CreateOrGetTexture("Data/Images/particle.png")); 

	m_mesh = new Mesh();
	MeshBuilder mb; 
	m_mesh->FromBuilderForType<VertexPCU>(mb);
	m_renderable = new Renderable(m_mesh, &m_transform, mat);
	scene->AddRenderable(m_renderable); 
	m_renderable->m_isLit = false;

	SetSpawnRate(30.f);
}

void ParticleEmitter::Update(Camera* cam, float dt)
{
	UNUSED(dt);

	if (m_spawnsOverTime)
	{
		//Hacky: to avoid first frame particles built up
		if (!m_burstSpawned)
		{
			m_spawnInterval.DecrementAll();
			m_burstSpawned = true;
		}
		else
		{
			uint particles = m_spawnInterval.DecrementAll();
			SpawnParticles(MinInt(m_maxParticles - (int) m_particles.size(), particles));
		}
	}
	else if (!m_burstSpawned)
	{
		SpawnParticles(m_maxParticles);
		m_burstSpawned = true;
	}

	float t = (float) GetCurrentTimeSeconds();

	uint particleCount = (uint) m_particles.size();  
	for (uint i = particleCount - 1U; i < particleCount; --i) 
	{
		particle_t &p = m_particles[i]; 
		//p.force = Vector3(0.0f, -9.8f, 0.0f); 
		p.Update(0.1f); 

		if (p.IsDead(t)) 
		{
			//quick remove
			size_t size = m_particles.size();
			m_particles[i] = m_particles[size - 1];
			m_particles.pop_back();
			i--;
		} 
	}

	//compensate for Renderable drawing mesh at model
  	Matrix44 temp = Matrix44::MakeInverseFast(m_transform.GetWorldMatrix());
 	temp.Append(cam->m_transform.GetWorldMatrix());

 	Vector3 right = temp.GetRight();
 	Vector3 up = temp.GetUp();
	particleCount = (uint) m_particles.size(); 

	MeshBuilder mb; 
	for (uint i = 0; i < particleCount; ++i) 
	{
		particle_t &p = m_particles[i]; 
		mb.AddPlane(p.position, right, up, AABB2(0, 0, p.size, p.size), AABB2::ZERO_TO_ONE, m_color);
	}

	m_mesh->FromBuilderForType<VertexPCU>(mb);
}

bool ParticleEmitter::IsReadyToCleanUp()
{
	return !m_spawnsOverTime && m_particles.size() == 0;
}

void ParticleEmitter::SpawnParticle()
{
	particle_t p; 

	//TODO: make this more robust later
	switch (m_emitterShape)
	{
	case EMITTER_SPHERE:
		p.velocity = RandomPointOnSphere() * m_velocityFactor; 
		p.position = Vector3::zero; 
		break;
	case EMITTER_CUBE:
		p.position = RandomPointInCube(m_shapeScale);
		p.velocity = Vector3::zero;
		break;
	}
  
	float lifetime = GetRandomFloatInRange(m_lifeTime.min, m_lifeTime.max); 
	p.timeBorn = (float) GetCurrentTimeSeconds(); 
	p.timeDead = p.timeBorn + lifetime; 

	p.size = GetRandomFloatInRange(m_size.min, m_size.max);
	p.force = m_force; 
	p.mass = 1.0f; 

	m_particles.push_back(p); 
}

void ParticleEmitter::SpawnParticles(uint count)
{
	for (uint i = 0; i < count; ++i) 
	{
		SpawnParticle(); 
	}
}

void ParticleEmitter::SetSpawnRate(float particlesPerSecond)
{
	if (particlesPerSecond == 0.0f) 
	{
		m_spawnsOverTime = false; 
	} 
	else 
	{
		m_spawnsOverTime = true; 
		m_spawnRate = particlesPerSecond;
		m_spawnInterval.SetTimer(1.0f / particlesPerSecond);
	}
}

void ParticleEmitter::Destroy()
{
	//hacky...make IsReadyToCleanUp fail
	m_spawnsOverTime = false;
	m_particles.clear();
}
