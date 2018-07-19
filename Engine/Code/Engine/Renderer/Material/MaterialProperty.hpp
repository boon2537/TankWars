#pragma once

#include "Engine/Renderer/glFunctions.hpp"

class MaterialProperty
{
public:
	MaterialProperty(std::string name)
		: m_name(name) {}

	virtual MaterialProperty* Clone() = 0;
	virtual void Bind(int bind_point) = 0;

public:
	std::string m_name;
	bool m_ignoreBindPoint = false;
};
