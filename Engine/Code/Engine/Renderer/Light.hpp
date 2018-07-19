#pragma once

#include "Engine/Core/Transform.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Renderer;
class Camera;
class Texture;

class Light
{
public:
	~Light();
	Light();

	void SetUpAsPointLight(const Rgba& color, float intensity, const Vector3& attenuation);
	void SetUpAsPointLight(const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation);
	void SetUpAsDirectional(const Rgba& color, float intensity, const Vector3& attenuation);
	void SetUpAsDirectional(const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation);
	void SetUpAsCone(const Rgba& color, float intensity, const Vector3& attenuation, float dotInnerAngle, float dotOuterAngle);
	void SetUpAsCone(const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation, float dotInnerAngle, float dotOuterAngle);

	bool IsDirectionalLight();
	Vector3 GetDirection();
	float GetAffectorRate(const Vector3& affectedPosition) const;

	//for shadow
	Texture* GetorCreateShadowTexture();

	void DebugRender();

public:
	Transform m_transform;
	bool m_usesShadow = true;

	Rgba m_color = Rgba::white;
	float m_intensity = 0.f;

	Vector3 m_attenuation;
	Vector3 m_spec_attunation;

	float m_directionFactor = 0.f;
	float m_dotInnerAngle = 0.f;
	float m_dotOuterAngle = 2.f * MathUtils::PI;

	Matrix44 m_shadowVP = Matrix44();

	//shadow stuff
	Texture* m_depthFromCamera = nullptr;
	uint m_depthTextureResolution = 1080; 
	float m_shadow_ppu = 256;
	float m_shadowDistance = 800; 
};
