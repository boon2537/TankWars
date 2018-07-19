#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Math/MathUtils.hpp"

void OrbitCamera::SetTarget(const Vector3 & new_target)
{
	m_target = new_target;
}

void OrbitCamera::SetSphericalCoordinate(float rad, float rot, float azi)
{
	m_radius = rad;
	m_rotation = rot;
	m_azimuth = azi;

	float eyeX = m_target.x + (m_radius * cosf(m_azimuth) * cosf(m_rotation));
	float eyeY = m_target.y + (m_radius * sinf(m_azimuth));
	float eyeZ = m_target.z + (m_radius * cosf(m_azimuth) * sinf(m_rotation));

	LookAt(Vector3(eyeX, eyeY, eyeZ), m_target); 
}
