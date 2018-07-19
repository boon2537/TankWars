#pragma once

#include "Engine/Renderer/Material/MaterialProperty.hpp"

class MaterialPropertyVector3 : public MaterialProperty
{
public:
	MaterialPropertyVector3(std::string name, const Vector3& data)
		: MaterialProperty(name), m_data(data) {}

	virtual MaterialProperty* Clone() override
	{
		return new MaterialPropertyVector3(m_name, m_data);
	}

	virtual void Bind(int bind_point) override
	{
		glUniform3fv(bind_point, 1, (const GLfloat*) &m_data);
	}

public:
	Vector3 m_data;
};
