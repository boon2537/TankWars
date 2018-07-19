#include "Game/Spawner.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Game/Terrain.hpp"
#include "Game/Projectile.hpp"
#include "Engine/Debug/DebugRender.hpp"

Spawner::~Spawner()
{
	g_theGame->m_renderScene->RemoveRenderable(m_renderable);
}

Spawner::Spawner(const Vector3& spawnPos, int id)
	: m_id(id)
{
	MeshBuilder mb;
	mb.AddCube(Vector3::zero, m_dimensions, Vector3::one, Rgba::white);
	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);

	Material* mat = Material::GetOrCreate("Data/Materials/asteroid.xml");

	float heightOffset = 9.f;
	m_transform.SetLocalPosition(spawnPos);
	m_transform.TranslateLocal(Vector3(0, heightOffset, 0));
	m_renderable = new Renderable(mesh, &m_transform, mat);
	g_theGame->m_renderScene->AddRenderable(m_renderable);

	m_bounds = AABB3::MakeFromDimensions(m_transform.GetLocalPosition(), m_dimensions);
	m_spawnStopWatch.SetClock(g_theGame->m_gameClock);
	m_spawnStopWatch.SetTimer(m_spawnInterval);

	SpawnEnemies();
}

void Spawner::Update(float deltaSeconds)
{
	if (CanSpawnNewEnemy() && m_spawnStopWatch.CheckAndReset())
	{
		SpawnEnemies();
	}
}

void Spawner::CheckProjectileOverlap(Projectile* p)
{
	//just do point vs AABB3
	if (m_bounds.IsContained(p->m_transform.GetWorldPosition()))
	{
		TakeDamage(p->m_damage);
		p->Destroy();
	}
}

void Spawner::TakeDamage(float damage)
{
	m_HP -= damage;
	if (m_HP <= 0)
	{
		m_isDead = true;
		AudioSystem::PlayOneOff("buildingDie");
	}
}

bool Spawner::CanSpawnNewEnemy()
{
	return m_currentSpawns < m_maxSpawns;
}

void Spawner::SpawnEnemies()
{
	float spawnExtends = 10.f;
	int enemiesToSpawn = MinFloat(m_spawnsPerInterval, m_maxSpawns - m_currentSpawns);
	for (int i = 0; i < enemiesToSpawn; i++)
	{
		Vector3 pos = m_transform.GetWorldPosition();
		g_theGame->SpawnEnemy(Vector3(GetRandomFloatInRange(pos.x - spawnExtends, pos.x + spawnExtends), -50.f, GetRandomFloatInRange(pos.z - spawnExtends, pos.z + spawnExtends)), this);
		m_currentSpawns++;
	}
}
