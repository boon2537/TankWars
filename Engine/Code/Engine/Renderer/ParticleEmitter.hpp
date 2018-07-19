#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <vector>

class Mesh;
class Camera;
class Renderable;

enum eEmitterShape
{
	EMITTER_SPHERE,
	EMITTER_CUBE,
};

struct particle_t
{
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float size;
	float mass;

	float timeBorn;
	float timeDead;

	void operator=(const particle_t& copyFrom)
	{
		position = copyFrom.position;
		velocity = copyFrom.velocity;
		force = copyFrom.force;
		size = copyFrom.size;
		mass = copyFrom.mass;
		timeBorn = copyFrom.timeBorn;
		timeDead = copyFrom.timeDead;
	}

	void Update(float dt)
	{
		//forward euler
		Vector3 accel = force / mass;
		velocity += accel * dt;
		position += velocity * dt;

		force = Vector3::zero;
	}

	inline bool IsDead(float time) { return time >= timeDead; }

	float GetNormalizedAge(float t)
	{
		return (t - timeBorn) / (timeDead - timeBorn);
	}
};

class ParticleEmitter
{
public:
	~ParticleEmitter();
	ParticleEmitter();

	void Update(Camera *cam, float dt); 
	bool IsReadyToCleanUp();
	void SpawnParticle(); 
	void SpawnParticles(uint count); 

	void SetSpawnRate(float particlesPerSecond); 
	inline float GetSpawnRate() { return m_spawnRate; }
	void Destroy();

public:
	Transform m_transform; 
	Renderable* m_renderable;

	Mesh* m_mesh; 
	MeshBuilder m_builder; 

	std::vector<particle_t> m_particles; 

	bool m_spawnsOverTime; 
	StopWatch m_spawnInterval; 
	IntRange m_burst; 

	eEmitterShape m_emitterShape = EMITTER_SPHERE;
	float m_shapeScale = 1.f;
	float m_velocityFactor = 1.f;
	Vector3 m_force = Vector3::zero;
	uint m_maxParticles = 1000;
	FloatRange m_lifeTime = FloatRange(1.f, 3.f);
	FloatRange m_size = FloatRange(0.1f, 0.2f);
	Rgba m_color = Rgba::white;

private:
	bool m_burstSpawned = false;
	float m_spawnRate = 0.f;
};
