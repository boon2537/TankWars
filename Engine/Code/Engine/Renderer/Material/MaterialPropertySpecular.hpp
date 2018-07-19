#pragma once

#include "Engine/Renderer/Material/MaterialProperty.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"

class MaterialPropertySpecular : public MaterialProperty
{
public:
	MaterialPropertySpecular(std::string name, float amount, float power)
		: MaterialProperty(name), m_amount(amount), m_power(power) { m_ignoreBindPoint = true; }

	virtual MaterialProperty* Clone() override
	{
		return new MaterialPropertySpecular(m_name, m_amount, m_power);
	}

	virtual void Bind(int bind_point) override
	{
		UNUSED(bind_point);
		Renderer* r = Renderer::GetInstance();
		r->SetSpecularConstants(m_amount, m_power);
	}

public:
	float m_amount;
	float m_power;
};
