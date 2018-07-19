#pragma once

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vector3.hpp"

// Camera that pivots around a target.
// Good for UI, or tactical RPGs
class OrbitCamera : public Camera
{
public:
	void SetTarget(const Vector3& new_target); 
	void SetSphericalCoordinate(float rad, float rot, float azi); 

public:
	Vector3 m_target; 

	float m_radius;      // distance from target
	float m_rotation;    // rotation around Y
	float m_azimuth;     // rotation toward up after previous rotation
};
