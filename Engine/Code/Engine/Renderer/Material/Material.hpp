#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include <string>
#include <vector>
#include <map>

class Texture;
class UniformBuffer;
class MaterialProperty;
class Shader;
class Sampler;

class Material
{
public:
	~Material();
	Material(Shader* shader);
	Material(const std::string& path);

	void LoadFromXML(const tinyxml2::XMLElement& element);
	Material* Clone();

	inline bool IsValid() const { return m_shader != nullptr; }

	void SetShader(Shader *shader); 
	Shader* GetSharedShader();
	Shader* GetShader();

	// Properties
	void SetTexture2D(const uint bind, Texture* texture);
	void SetSampler(const uint bind, Sampler* sampler); 
	Texture* GetTexture2D(const uint index); 
	Sampler* GetSampler(const uint index); 

	// Property setters - add more as you need them
	void SetProperty(const char* name, float value);
	void SetProperty(const char* name, const Vector2& value);
	void SetProperty(const char* name, const Vector3& value);
	void SetProperty(const char* name, const Vector4& value); 
	void SetProperty(const char* name, const Rgba& value); 

	// Special case helpers that are common properties
	void SetTint(const Rgba& tint); 
	void SetSpecular(float amount, float power); 

	void RemoveProperty(const char* name);

public:
	static Material* CreateInstance(const std::string& name, Shader* shader);
	static Material* GetOrCreate(const std::string& path);

public:
	std::string m_name = "";
	std::map<uint, Texture*> m_textures;
	std::map<uint, Sampler*> m_samplers;
	std::vector<MaterialProperty*> m_properties;

	bool m_is_resource = false; //false if this is an instance

private:
	Shader* m_shader = nullptr;
	Shader* m_sharedShader = nullptr;
};
