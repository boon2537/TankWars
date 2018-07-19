#pragma once

#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Rgba.hpp"

class Renderable;
class ParticleEmitter;
class Camera;
class Mesh;
class Terrain;

class Ship
{
public:
	~Ship();
	Ship();

	void UpdateMovement(float deltaSeconds);
	void UpdateTarget(float deltaSeconds);
	void ApplyStickToTerrain(Terrain* terrain);
	void TakeDamage(int damage);
	void Respawn(const Vector3& pos);
	inline bool IsAlive() { return m_currentHP > 0; };

	Vector2 Get_XZ_pos();

public:
	Transform m_transform;
	float m_shootPerSecond = 4.f;
	int m_currentHP = 100;
	int m_maxHP = 100;
	float m_physicsRadius = 3.f;

	Renderable* m_renderable = nullptr;
	ParticleEmitter* m_exhaust = nullptr;
	
	//charging
	float m_chargeTime = 1.f;
	StopWatch m_chargeWatch;
	ParticleEmitter* m_chargeParticle = nullptr;
	Rgba m_chargeBeginColor = Rgba::white;
	Rgba m_fullyChargeColor = Rgba::red;

	StopWatch m_shootWatch;

	Transform m_turrentTransform;
	Transform m_bulletSpawnTransform;
	Renderable* m_turrentRenderable = nullptr;

	Vector3 m_target;

	float m_playHeightOffset = 2.f;
	float m_currentOffset;
	float m_respawnOffset = 150.f;
	bool m_isFalling = false;
	float m_fallAirTime = 8.f;
	float m_timeToHitGround;
};
