#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Core/Window.hpp"

Light::~Light()
{
	delete m_depthFromCamera;
}

Light::Light()
{
}

void Light::SetUpAsPointLight(const Rgba& color, float intensity, const Vector3& attenuation)
{
	SetUpAsPointLight(color, intensity, attenuation, attenuation);
}

void Light::SetUpAsPointLight(const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation)
{
	m_color = color;
	m_intensity = intensity;
	m_attenuation = attenuation;
	m_spec_attunation = spec_attunation;
	m_directionFactor = 0.f;
	m_dotInnerAngle = 1.f;
	m_dotOuterAngle = -1.f;
}

void Light::SetUpAsDirectional(const Rgba& color, float intensity, const Vector3& attenuation)
{
	SetUpAsDirectional(color, intensity, attenuation, attenuation);
}

void Light::SetUpAsDirectional(const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation)
{
	m_color = color;
	m_intensity = intensity;
	m_attenuation = attenuation;
	m_spec_attunation = spec_attunation;
	m_directionFactor = 1.0f;
	m_dotInnerAngle = 1.f;
	m_dotOuterAngle = -1.f;
}

void Light::SetUpAsCone(const Rgba& color, float intensity, const Vector3& attenuation, float dotInnerAngle, float dotOuterAngle)
{
	SetUpAsCone(color, intensity, attenuation, attenuation, dotInnerAngle, dotOuterAngle);
}

void Light::SetUpAsCone(const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation, float dotInnerAngle, float dotOuterAngle)
{
	m_color = color;
	m_intensity = intensity;
	m_attenuation = attenuation;
	m_spec_attunation = spec_attunation;
	m_directionFactor = 1.0f;
	m_dotInnerAngle = dotInnerAngle;
	m_dotOuterAngle = dotOuterAngle;
}

bool Light::IsDirectionalLight()
{
	return m_directionFactor == 1.f;
}

Vector3 Light::GetDirection()
{
	return m_transform.GetWorldMatrix().GetEuler();
}

float Light::GetAffectorRate(const Vector3& affectedPosition) const
{
	//now assume everything is point light
	float distanceAway = GetDistance(m_transform.GetWorldPosition(), affectedPosition);
	return m_intensity / (m_attenuation.x + (m_attenuation.y * distanceAway) + (distanceAway * distanceAway * m_attenuation.z));
}

Texture* Light::GetorCreateShadowTexture()
{
	if (m_depthFromCamera == nullptr)
	{
		Renderer* r = Renderer::GetInstance();
		m_depthFromCamera =  r->CreateDepthStencilTarget(m_depthTextureResolution, m_depthTextureResolution);
	}

	return m_depthFromCamera;
}

void Light::DebugRender()
{
	DebugRenderBasis(0, m_transform.GetWorldMatrix());
}
