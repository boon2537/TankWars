#pragma once

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Transform.hpp"

class Renderable;
class Mesh;
class Light;

class Projectile
{
public:
	~Projectile();
	Projectile(const Vector3& spawnPos, const Vector3& direction);

	void Update(float deltaSeconds);
	inline bool IsDead() { return m_dieClock.HasElapsed() || m_hitEnemy; }
	void Destroy();

	//hacky chargedshot
	void SetAsChagedShot(const Vector3& intendedDest);

public:
	Transform m_transform;
	Mesh* m_mesh = nullptr;
	Renderable* m_renderable = nullptr;
	Light* m_light = nullptr;
	Vector3 m_direction;
	
	float m_speed = 80.f;
	float m_timeToLive = 10.f;
	StopWatch m_dieClock;

	bool m_hitEnemy = false;
	float m_radius = 0.5f;
	int m_damage = 10;

	//hacky chargedshot
	Vector3 m_intendedDest;
	bool m_isChargedShot = false;
	float m_chargedShotScale = 3.f;
	float m_chargedShotRadius = 20.f;
};
