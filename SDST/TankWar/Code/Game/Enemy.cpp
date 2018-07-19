#include "Game/Enemy.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Game/Terrain.hpp"
#include "Game/Projectile.hpp"
#include "Game/Spawner.hpp"
#include "Engine/Profiler/Profiler.hpp"

Enemy::~Enemy()
{
	g_theGame->m_renderScene->RemoveRenderable(m_renderable);
	g_theGame->m_renderScene->RemoveRenderable(m_faceRenderable);
}

Enemy::Enemy(const Vector3& spawnPos, int spawnerID)
	: m_spawnerID(spawnerID)
{
	MeshBuilder mb;
	mb.AddUVSphere(Vector3::zero, m_radius, 32, 16, Rgba::white);
	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);

	Material* mat = Material::GetOrCreate("Data/Materials/asteroid.xml");

	m_transform.SetLocalPosition(spawnPos);
	m_renderable = new Renderable(mesh, &m_transform, mat);
	g_theGame->m_renderScene->AddRenderable(m_renderable);

	mb.Reset();
	mb.AddPlane(Vector3(0, 0, m_radius), Vector3(-1, 0, 0), Vector3(0, 1, 0), AABB2(m_radius * -0.5f, m_radius * -0.5f, m_radius * 0.5f, m_radius * 0.5f));
	mat = Material::GetOrCreate("Data/Materials/enemyFace.xml");
	mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);
	m_faceRenderable = new Renderable(mesh, &m_transform, mat);
	g_theGame->m_renderScene->AddRenderable(m_faceRenderable);
}

void Enemy::Update(float deltaSeconds)
{
	ProfilerPush(__FUNCTION__);

	float heightOffset = 2.0f;
	Vector3 pos = m_transform.GetWorldPosition();
	float height = g_theGame->m_terrain->GetHeight(Vector2(pos.x, pos.z)) + heightOffset; 
	pos.y = height;
	m_transform.SetLocalPosition(pos); 

	ApplyForce(m_flockBehavior.CalculateSeperateForce(g_theGame->m_enemies, this) * m_flockBehavior.m_seperateForceWeight);
	ApplyForce(m_flockBehavior.CalculateAlignmentForce(g_theGame->m_enemies, this) * m_flockBehavior.m_alighmentForceWeight);
	ApplyForce(m_flockBehavior.CalculateCohesionForce(g_theGame->m_enemies, this) * m_flockBehavior.m_cohesionForceWeight);
	ApplyForce(m_flockBehavior.CalculateSeekForce(g_theGame->m_ship->Get_XZ_pos(), this) * m_flockBehavior.m_seekForceWeight);

	m_velocity += m_acceleration;
	m_velocity.x = MinFloat(m_velocity.x, m_maxSpeed);
	m_velocity.y = MinFloat(m_velocity.y, m_maxSpeed);
	m_acceleration = Vector2::zero;

	Vector2 moveFactor = m_velocity * deltaSeconds * m_maxSpeed;
	m_transform.TranslateLocal(Vector3(moveFactor.x, 0, moveFactor.y));

	//turn toward player
	Matrix44 lookAt = Matrix44::LookAt(m_transform.GetWorldPosition(), g_theGame->m_ship->m_transform.GetWorldPosition(), m_transform.GetLocalMatrix().GetUp());
	float turnSpeed = 4.f;
	float turnThisFrame = turnSpeed * deltaSeconds;
	m_transform.SetWorldMatrix(Matrix44::TurnToward(m_transform.GetWorldMatrix(), lookAt, turnThisFrame));

	ProfilerPop();
}

void Enemy::ApplyForce(const Vector2& force)
{
	m_acceleration += force;
}

void Enemy::CheckProjectileOverlap(Projectile* p)
{
	if (DoSpheresOverlap(m_transform.GetLocalPosition(), m_radius, p->m_transform.GetLocalPosition(), p->m_radius))
	{
		TakeDamage(p->m_damage);
		p->Destroy();
	}
}

Vector2 Enemy::Get_XY_Pos()
{
	return Vector2(m_transform.GetWorldPosition().x, m_transform.GetWorldPosition().z);
}

void Enemy::Destroy()
{
	m_isDead = true;
	AudioSystem::PlayOneOff("enemyDie");

	//hacky OnDestroy
	if (g_theGame->IsSpawnerAlive(m_spawnerID))
	{
		g_theGame->m_spawners[m_spawnerID]->m_currentSpawns--;
	}
}

void Enemy::TakeDamage(float damage)
{
	m_HP -= damage;
	if (m_HP <= 0)
	{
		Destroy();
	}
}
