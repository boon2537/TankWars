#pragma once

#include "Engine/Renderer/Material/MaterialProperty.hpp"

class MaterialPropertyFloat : public MaterialProperty
{
public:
	MaterialPropertyFloat(std::string name, float data)
		: MaterialProperty(name), m_data(data) {}

	virtual MaterialProperty* Clone() override
	{
		return new MaterialPropertyFloat(m_name, m_data);
	}

	virtual void Bind(int bind_point) override
	{
		glUniform1fv(bind_point, 1, (const GLfloat*) &m_data);
	}

public:
	float m_data;
};
