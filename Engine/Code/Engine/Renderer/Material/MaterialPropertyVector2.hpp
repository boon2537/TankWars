#pragma once

#include "Engine/Renderer/Material/MaterialProperty.hpp"
#include "Engine/Math/Vector2.hpp"

class MaterialPropertyVector2 : public MaterialProperty
{
public:
	MaterialPropertyVector2(std::string name, const Vector2& data)
		: MaterialProperty(name), m_data(data) {}

	virtual MaterialProperty* Clone() override
	{
		return new MaterialPropertyVector2(m_name, m_data);
	}

	virtual void Bind(int bind_point) override
	{
		glUniform2fv(bind_point, 1, (const GLfloat*) &m_data);
	}

public:
	Vector2 m_data;
};
