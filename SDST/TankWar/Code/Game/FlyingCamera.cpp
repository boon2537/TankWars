#include "Game/FlyingCamera.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Game/GameCommon.hpp"

//constexpr float ROTATIONAL_SPEED = 0.5f;
constexpr float ROTATIONAL_SPEED = 0.5f;
constexpr float MOVEMENT_SPEED = 10.0f;

FlyingCamera::~FlyingCamera()
{
}

FlyingCamera::FlyingCamera()
{
	m_camera = new Camera();
	m_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_camera->SetProjectionPerspective(60.f, 16.f / 9.f, 1, 800);
	m_camera->SetLocalRotationEuler(Vector3(0, 0, 0));
}

void FlyingCamera::Update(float deltaSeconds)
{
	//Apply Rotation
	Vector2 mouse_delta = g_theInput->GetMouseDelta();

	Vector3 local_euler = Vector3::zero;
	local_euler.y = mouse_delta.x * ROTATIONAL_SPEED * deltaSeconds;
	local_euler.x = mouse_delta.y * ROTATIONAL_SPEED * deltaSeconds;

	m_camera->RotateLocalByEuler(local_euler);
	Vector3 currentEuler = m_camera->m_transform.GetLocalEulerAngles();

	Vector3 eulerToSet = Vector3::zero;
	eulerToSet.x = ClampFloat(currentEuler.x, -MathUtils::PI/2.0f, MathUtils::PI/2.0f);
	eulerToSet.y = BetterMod(currentEuler.y, 2.0f * MathUtils::PI); //better mod handle wrap case from negative?

	m_camera->SetLocalRotationEuler(eulerToSet);

	//Apply movement
	float left_right = 0;
	float forward_back = 0;

	if (g_theInput->IsKeyPressed(KEY_CODE::W))
		forward_back += 1.f;
	if (g_theInput->IsKeyPressed(KEY_CODE::S))
		forward_back -= 1.f;
	if (g_theInput->IsKeyPressed(KEY_CODE::D))
		left_right += 1.f;
	if (g_theInput->IsKeyPressed(KEY_CODE::A))
		left_right -= 1.f;

	Vector3 cam_forward = m_camera->GetForward();
	Vector3 cam_right = m_camera->GetRight();

	Vector3 world_offset = (cam_right * left_right * MOVEMENT_SPEED * deltaSeconds) + (cam_forward * forward_back * MOVEMENT_SPEED * deltaSeconds);

	m_camera->TranslateLocal(world_offset);
}
