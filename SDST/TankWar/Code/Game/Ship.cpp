#include "Game/Ship.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/VertexLit.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contact.hpp"
#include "Game/Projectile.hpp"
#include "Game/Terrain.hpp"
#include "Engine/Profiler/ProfileLogScoped.hpp"
#include "Engine/Profiler/Profiler.hpp"

constexpr float ROTATIONAL_SPEED = 1.4f;
constexpr float MOVEMENT_SPEED = 18.f;
constexpr float TURRENT_TURN_SPEED = 6.f;

Ship::~Ship()
{
	g_theGame->m_renderScene->RemoveRenderable(m_renderable);
	g_theGame->m_renderScene->RemoveRenderable(m_turrentRenderable);
	m_exhaust->Destroy();
	m_chargeParticle->Destroy();
}

Ship::Ship()
{
	Material* shipMat = Material::GetOrCreate("Data/Materials/ship.xml");

	MeshBuilder mb;
	mb.AddCube(Vector3::zero, Vector3(3.f, 3.f, 5.f), Vector3::one, Rgba::white);
	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);
	m_renderable = new Renderable(mesh, &m_transform, shipMat);
	g_theGame->m_renderScene->AddRenderable(m_renderable);

	mb.Reset();
	mb.AddUVSphere(Vector3::zero, 1.0f, 32, 16, Rgba::white);
	mb.AddCube(Vector3(0.f, 0.f, 2.5f), Vector3(0.6f, 0.6f, 5.f), Vector3::one, Rgba::white);
	mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);
	m_turrentTransform.MakeChild(&m_transform);
	m_turrentTransform.TranslateLocal(Vector3(0.f, 2.5f, 0.f));
	m_bulletSpawnTransform.TranslateLocal(Vector3(0, 0, 5));
	m_bulletSpawnTransform.MakeChild(&m_turrentTransform);

	m_turrentRenderable = new Renderable(mesh, &m_turrentTransform, shipMat);
	m_turrentRenderable->GetMaterial()->SetTint(Rgba::red);
	g_theGame->m_renderScene->AddRenderable(m_turrentRenderable);

	m_exhaust = new ParticleEmitter();
	m_exhaust->m_emitterShape = EMITTER_CUBE;
	m_exhaust->m_shapeScale = 0.4f;
	m_exhaust->m_force = Vector3(0, 0, -8.f);
	m_exhaust->SetSpawnRate(20.f);
	m_exhaust->m_lifeTime = FloatRange(2.f, 2.5f);
	m_exhaust->m_size = FloatRange(0.2f, 0.25f);
	m_exhaust->m_color = Rgba::blue;
	m_exhaust->m_transform.MakeChild(&m_transform);
	m_exhaust->m_transform.SetLocalPosition(Vector3(0, 0, -3.f));
	g_theGame->m_particleSystem->AddEmitter(m_exhaust);

	m_chargeWatch.SetClock(g_theGame->m_gameClock);
	m_chargeWatch.SetTimer(m_chargeTime);
	m_chargeParticle = new ParticleEmitter();
	m_chargeParticle->m_emitterShape = EMITTER_SPHERE;
	m_chargeParticle->m_shapeScale = 0.2f;
	m_chargeParticle->SetSpawnRate(800.f);
	m_chargeParticle->m_lifeTime = FloatRange(0.f);
	m_chargeParticle->m_color = Rgba::red;
	m_chargeParticle->m_transform.MakeChild(&m_bulletSpawnTransform);
	m_chargeParticle->m_transform.SetLocalPosition(Vector3::zero);
	g_theGame->m_particleSystem->AddEmitter(m_chargeParticle);

	m_shootWatch.SetClock(g_theGame->m_gameClock);
	m_shootWatch.SetTimer(1 / m_shootPerSecond);

	m_currentHP = m_maxHP;
	m_currentOffset = m_playHeightOffset;
}

void Ship::UpdateMovement(float deltaSeconds)
{
	if (!IsAlive())
		return;

	//Apply movement
	float forward_back = 0;

	if (g_theInput->IsKeyPressed(KEY_CODE::W))
		forward_back += 1.f;
	if (g_theInput->IsKeyPressed(KEY_CODE::S))
		forward_back -= 1.f;

	Vector3 forward = m_transform.GetLocalMatrix().GetForward();
	Vector3 world_offset = forward * forward_back * MOVEMENT_SPEED * deltaSeconds;
	m_transform.TranslateLocal(world_offset);

	//Apply rotate
	Vector3 local_euler = Vector3::zero;
	if (g_theInput->IsKeyPressed(KEY_CODE::D))
		local_euler.y += ROTATIONAL_SPEED * deltaSeconds;
	if (g_theInput->IsKeyPressed(KEY_CODE::A))
		local_euler.y -= ROTATIONAL_SPEED * deltaSeconds;

	m_transform.RotateLocalByEuler(local_euler);

	if (g_theInput->WasMouseJustPressed(MOUSE_CODE::BUTTON_LEFT))
	{
		m_chargeWatch.Reset();
		m_chargeParticle->m_lifeTime = FloatRange(0.25f, 0.5f);
		m_chargeParticle->m_color = Rgba::white;
	}
	if (m_chargeWatch.HasElapsed())
	{
		m_chargeParticle->m_color = Rgba::red;
	}
	//Spawn Projectile
	if (g_theInput->WasMouseJustReleased(MOUSE_CODE::BUTTON_LEFT) && m_shootWatch.CheckAndReset())
	{
		Projectile* p = new Projectile(m_bulletSpawnTransform.GetWorldPosition(), m_turrentTransform.GetWorldMatrix().GetForward());
		g_theGame->m_projectiles.push_back(p);

		if (m_chargeWatch.CheckAndReset())
		{
			p->SetAsChagedShot(m_target);
		}

		AudioSystem::PlayOneOff("tankShoot");
		m_chargeParticle->m_lifeTime = FloatRange(0);
	}

	if (forward_back > 0 && m_exhaust->GetSpawnRate() != 100.f)
	{
		m_exhaust->SetSpawnRate(100.f);
	}
	else if (forward_back <= 0 && m_exhaust->GetSpawnRate() != 20.f)
	{
		m_exhaust->SetSpawnRate(20.f);
	}
}

void Ship::UpdateTarget(float deltaSeconds)
{
	if (!IsAlive())
		return;

	ProfilerPush(__FUNCTION__);

	// raycast against the world (this could eventually live in a physics system, but we're not going to generalize that much yet)
	// first, we need a ray.  We can use our camera's forward as the direction, and our camera's position as the origin; 
	float cameraForwardFactor = 1.05f;
	Vector3 camPos = g_theGame->m_gameCamera->m_transform.GetWorldPosition();
	Ray3 ray = Ray3(camPos + (g_theGame->m_gameCamera->GetForward() * cameraForwardFactor), g_theGame->m_gameCamera->GetForward()); 

	// This returns the number of contacts hit, and takes 
	// an array to store the contacts in, and the max number of contacts I care about
	// as well as the ray to use; 
	RayCastHit3 contact; 
	if (g_theGame->Raycast(&contact, ray)) 
	{
		m_target = contact.position; 
		//DebugLogf("hit", Rgba::red, 0);
	}
	else 
	{
		// didn't hit anything, just pick something far along the ray
		m_target = ray.Evaluate(1000.0f); 
	}

	DebugRenderLineSegment(0, camPos, Rgba::red, m_target, Rgba::red);
	DebugRenderPoint(0, m_target, Rgba::red, Rgba::red);

	DebugRenderLineSegment(0, m_target, Rgba::red,  m_turrentTransform.GetWorldPosition(), Rgba::red);


	Matrix44 lookAt = Matrix44::LookAt(m_turrentTransform.GetWorldPosition(), m_target, m_transform.GetLocalMatrix().GetUp());
	float turnThisFrame = TURRENT_TURN_SPEED * deltaSeconds;

	m_turrentTransform.SetWorldMatrix(Matrix44::TurnToward(m_turrentTransform.GetWorldMatrix(), lookAt, turnThisFrame));

	ProfilerPop();
}

void Ship::ApplyStickToTerrain(Terrain* terrain)
{
	//Update offset
	if (m_isFalling)
	{
		float currentTime = static_cast<float>(g_theGame->m_gameClock->GetClockCurrentTime());
		m_currentOffset = Interpolate(m_playHeightOffset, m_respawnOffset, SmoothStop3((m_timeToHitGround - currentTime) / m_fallAirTime));

		if (currentTime >= m_timeToHitGround)
		{
			m_currentOffset = m_playHeightOffset;
			m_isFalling = false;
		}
	}

	//Clamp ship y
	Vector3 pos = m_transform.GetWorldPosition();
	float height = terrain->GetHeight(Vector2(pos.x, pos.z)) + m_currentOffset; 
	pos.y = height;
	m_transform.SetLocalPosition(pos); 

	//stick to normal
	if (!m_isFalling)
	{
		Vector3 normal = terrain->GetNormalForXZ(Vector2(pos.x, pos.z)).GetNormalized();
		Vector3 anchorForward = m_transform.GetWorldMatrix().GetForward();
		anchorForward.NormalizeAndGetLength();

		Vector3 right = CrossProduct(normal, anchorForward);
		right.NormalizeAndGetLength();
		Vector3 newForward = CrossProduct(right, normal);
		newForward.NormalizeAndGetLength();

		Matrix44 newMat = Matrix44(right, normal, newForward, m_transform.GetWorldPosition());
		m_transform.SetLocalMatrix(newMat);
	}
}

void Ship::TakeDamage(int damage)
{
	if (g_theGame->m_DEBUG)
		return;

	m_currentHP = MaxInt(0, m_currentHP - damage);
	if (m_currentHP <= 0)
	{
		m_exhaust->SetSpawnRate(0.1f);
		g_theGame->SetDefeat();
	}
}

void Ship::Respawn(const Vector3& pos)
{
	m_currentHP = m_maxHP;
	m_transform.SetLocalPosition(pos);
	m_exhaust->SetSpawnRate(20.f);

	//start falling mode
	m_isFalling = true;
	m_timeToHitGround = static_cast<float>(g_theGame->m_gameClock->GetClockCurrentTime()) + m_fallAirTime;
	m_currentOffset = m_respawnOffset;
	m_transform.SetLocalRotationEuler(Vector3::zero);
}

Vector2 Ship::Get_XZ_pos()
{
	return Vector2(m_transform.GetWorldPosition().x, m_transform.GetWorldPosition().z);
}
