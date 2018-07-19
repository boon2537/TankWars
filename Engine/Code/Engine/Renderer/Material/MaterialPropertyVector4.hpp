#pragma once

#include "Engine/Renderer/Material/MaterialProperty.hpp"

class MaterialPropertyVector4 : public MaterialProperty
{
public:
	MaterialPropertyVector4(std::string name, const Vector4& data)
		: MaterialProperty(name), m_data(data) {}

	virtual MaterialProperty* Clone() override
	{
		return new MaterialPropertyVector4(m_name, m_data);
	}

	virtual void Bind(int bind_point) override
	{
		glUniform4fv(bind_point, 1, (const GLfloat*) &m_data);
	}

public:
	Vector4 m_data;
};
