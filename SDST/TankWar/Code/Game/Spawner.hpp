#pragma once

#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Math/AABB3.hpp"

class Renderable;
class Projectile;

class Spawner
{
public:
	~Spawner();
	Spawner(const Vector3& spawnPos, int id);

	void Update(float deltaSeconds);
	void CheckProjectileOverlap(Projectile* p);
	inline bool IsDead() { return m_isDead; }

	void TakeDamage(float damage);
	bool CanSpawnNewEnemy();
	void SpawnEnemies();

public:
	Transform m_transform;
	Renderable* m_renderable = nullptr;

	Vector3 m_dimensions = Vector3(6, 18, 6);
	AABB3 m_bounds;

	int m_HP = 50;
	bool m_isDead = false;

	int m_id;
	int m_maxSpawns = 20;
	int m_spawnsPerInterval = 4;
	int m_currentSpawns;
	StopWatch m_spawnStopWatch;
	float m_spawnInterval = 10.f;
};
