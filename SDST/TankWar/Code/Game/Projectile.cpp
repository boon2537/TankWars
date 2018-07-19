#include "Game/Projectile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Game/Terrain.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Enemy.hpp"
#include "Game/Spawner.hpp"

Projectile::~Projectile()
{
	g_theGame->m_renderScene->RemoveRenderable(m_renderable);
	g_theGame->m_renderScene->RemoveLight(m_light);
}

Projectile::Projectile(const Vector3& spawnPos, const Vector3& direction)
	: m_direction(direction)
{
	m_transform.SetLocalPosition(spawnPos);
	m_dieClock.SetClock(g_theGame->m_gameClock);
	m_dieClock.SetTimer(m_timeToLive);

	MeshBuilder mb;
	mb.AddUVSphere(Vector3::zero, m_radius, 32, 16, Rgba::white);

	m_mesh = new Mesh();
	m_mesh->FromBuilderForType<VertexPCU>(mb);

	m_renderable = new Renderable(m_mesh, &m_transform, g_theGame->m_projectileMat);
	m_renderable->m_isLit = true;

	m_light = new Light();
	m_light->m_transform.MakeChild(&m_transform);
	m_light->SetUpAsPointLight(Rgba::red, 1.f, Vector3(2.f, 0.2f, 0.2));

	g_theGame->m_renderScene->AddRenderable(m_renderable);
	g_theGame->m_renderScene->AddLight(m_light);
}

void Projectile::Update(float deltaSeconds)
{
	m_transform.TranslateLocal(m_direction * m_speed * deltaSeconds);

	//kill yourself if is below terrain
	Vector3 pos = m_transform.GetWorldPosition();
	if (pos.y < g_theGame->m_terrain->GetHeight(Vector2(pos.x, pos.z)))
	{
		Destroy();
	}
	else if (m_isChargedShot && GetDistance(pos, m_intendedDest) < 0.1f)
	{
		Destroy();
	}
}

void Projectile::Destroy()
{
	m_hitEnemy = true;

	//chargedShot aoe
	if (m_isChargedShot)
	{
		for each (Enemy* s in g_theGame->m_enemies)
		{
			if (GetDistance(m_transform.GetWorldPosition(), s->m_transform.GetWorldPosition()) <= m_chargedShotRadius)
			{
				s->TakeDamage(m_damage);
			}
		}

		for each (Spawner* s in g_theGame->m_spawners)
		{
			if (s != nullptr)
			{
				if (GetDistance(m_transform.GetWorldPosition(), s->m_transform.GetWorldPosition()) <= m_chargedShotRadius)
					s->TakeDamage(m_damage);
			}
		}

		DebugRenderSphere(0.3f, m_transform.GetWorldPosition(), m_chargedShotRadius, Rgba::red);
	}
}

void Projectile::SetAsChagedShot(const Vector3& intendedDest)
{
	m_intendedDest = intendedDest;
	m_transform.SetLocalScale(Vector3(m_chargedShotScale));
	m_isChargedShot = true;
}
