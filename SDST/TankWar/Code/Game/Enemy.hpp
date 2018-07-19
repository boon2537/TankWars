#pragma once

#include "Engine/Core/Transform.hpp"
#include "Game/FlockBehavior.hpp"

class Renderable;
class Projectile;

class Enemy
{
public:
	~Enemy();
	Enemy(const Vector3& spawnPos, int spawnerID);

	void Update(float deltaSeconds);
	void ApplyForce(const Vector2& force);
	void CheckProjectileOverlap(Projectile* p);
	Vector2 Get_XY_Pos();

	inline bool IsDead() { return m_isDead; }
	void Destroy();

	void TakeDamage(float damage);

public:
	Transform m_transform;
	Renderable* m_renderable = nullptr;
	Renderable* m_faceRenderable = nullptr;

	Vector2 m_velocity = Vector2::zero;
	Vector2 m_acceleration = Vector2::zero;
	float m_maxSpeed = 2.5f;
	float m_maxForce = 0.1f;
	FlockBehavior m_flockBehavior;

	float m_radius = 2.f;
	int m_HP = 10;
	int m_damage = 10;
	bool m_isDead = false;
	int m_spawnerID;
};
