#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Material/MaterialProperty.hpp"
#include "Engine/Renderer/Material/MaterialPropertyFloat.hpp"
#include "Engine/Renderer/Material/MaterialPropertySpecular.hpp"
#include "Engine/Renderer/Material/MaterialPropertyVector2.hpp"
#include "Engine/Renderer/Material/MaterialPropertyVector3.hpp"
#include "Engine/Renderer/Material/MaterialPropertyVector4.hpp"

static std::map<std::string, Material*> g_materialMap;

Material::~Material()
{
	delete m_shader;
	m_shader = nullptr;
	m_sharedShader = nullptr;

	for (uint i = 0; i < m_properties.size(); i++)
		delete m_properties[i];

	m_properties.clear();
}

Material::Material(Shader* shader)
{
	if (shader != nullptr)
		SetShader(shader);
}

Material::Material(const std::string& path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());

	LoadFromXML(*doc.RootElement());
}

void Material::LoadFromXML(const tinyxml2::XMLElement& element)
{
	Renderer* r = Renderer::GetInstance();
	ASSERT_RECOVERABLE(r != nullptr, "Load up renderer first!");

	m_name = ParseXmlAttribute(element, "name", m_name);

	const tinyxml2::XMLElement* elementToUse = element.FirstChildElement();
	int loopCount = 0;
	while (elementToUse != nullptr)
	{
		std::string elementName = elementToUse->Name();

		if (elementName == "shader")
		{
			Shader* s = r->GetShaderByID(ParseXmlAttribute(*elementToUse, "id", ""));
			ASSERT_RECOVERABLE(s != nullptr, "Shader not found!");
			ASSERT_RECOVERABLE(loopCount == 0, "<shader> element should be first, so data does not get override by the default values");
			SetShader(s);

			Material* modelMat = s->m_defaultMaterial;
			if (modelMat != nullptr)
			{				
				std::map<uint, Texture*>::iterator tex_it;
				for (tex_it = modelMat->m_textures.begin(); tex_it != modelMat->m_textures.end(); tex_it++)
					m_textures[tex_it->first] = tex_it->second;

				std::map<uint, Sampler*>::iterator sam_it;
				for (sam_it = modelMat->m_samplers.begin(); sam_it != modelMat->m_samplers.end(); sam_it++)
					m_samplers[sam_it->first] = sam_it->second;

				for (uint i = 0; i < modelMat->m_properties.size(); i++)
					m_properties.push_back(modelMat->m_properties[i]->Clone());
			}
		}
		else if (elementName == "texture")
		{
			TODO("set up sampler mode here");
			int bindPoint = ParseXmlAttribute(*elementToUse, "bind", 0);
			Texture* tex = r->CreateOrGetTexture(ParseXmlAttribute(*elementToUse, "src", ""));
			ASSERT_RECOVERABLE(tex != nullptr, "Texture not found!");

			SetSampler(bindPoint, r->GetDefaultSampler());
			SetTexture2D(bindPoint, tex);
		}
		else if (elementName == "float")
		{
			std::string name = ParseXmlAttribute(*elementToUse, "bind", nullptr);
			SetProperty(name.c_str(), ParseXmlAttribute(*elementToUse, "value", 0.f));
		}
		else if (elementName == "vec2")
		{
			std::string name = ParseXmlAttribute(*elementToUse, "bind", nullptr);
			SetProperty(name.c_str(), ParseXmlAttribute(*elementToUse, "value", Vector2::zero));
		}
		else if (elementName == "vec3")
		{
			std::string name = ParseXmlAttribute(*elementToUse, "bind", nullptr);
			SetProperty(name.c_str(), ParseXmlAttribute(*elementToUse, "value", Vector3::zero));
		}
		else if (elementName == "vec4")
		{
			std::string name = ParseXmlAttribute(*elementToUse, "bind", nullptr);
			SetProperty(name.c_str(), ParseXmlAttribute(*elementToUse, "value", Vector4::zero));
		}
		else if (elementName == "color")
		{
			std::string name = ParseXmlAttribute(*elementToUse, "bind", nullptr);
			SetProperty(name.c_str(), ParseXmlAttribute(*elementToUse, "value", Rgba::white));
		}
		else if (elementName == "spec")
		{
			SetSpecular(ParseXmlAttribute(*elementToUse, "amount", 0.7f), ParseXmlAttribute(*elementToUse, "power", 1.5f));
		}

		elementToUse = elementToUse->NextSiblingElement();
		loopCount++;
	}
}

Material* Material::Clone()
{
	//deep copy
	Material* newMat = new Material(m_shader);
	newMat->m_sharedShader = m_sharedShader;

	std::map<uint, Texture*>::iterator tex_it;
	for (tex_it = m_textures.begin(); tex_it != m_textures.end(); tex_it++)
		newMat->m_textures[tex_it->first] = tex_it->second;

	std::map<uint, Sampler*>::iterator sam_it;
	for (sam_it = m_samplers.begin(); sam_it != m_samplers.end(); sam_it++)
		newMat->m_samplers[sam_it->first] = sam_it->second;

	for (uint i = 0; i < m_properties.size(); i++)
		newMat->m_properties.push_back(m_properties[i]->Clone());

	return newMat;
}

void Material::SetShader(Shader* shader)
{
	if (shader->m_is_resource)
	{
		m_sharedShader = shader;
		m_shader = nullptr;
	}
	else
	{
		m_shader = shader;
	}
}

Shader* Material::GetSharedShader()
{
	return m_sharedShader;
}

Shader* Material::GetShader()
{
	if (m_shader == nullptr) 
	{
		m_shader = m_sharedShader->Clone(); 
		m_shader->m_is_resource = false;
	}

	return m_shader; 
}

void Material::SetTexture2D(const uint bind, Texture* texture)
{
	m_textures[bind] = texture;
}

void Material::SetSampler(const uint bind, Sampler* sampler)
{
	m_samplers[bind] = sampler;
}

Texture* Material::GetTexture2D(const uint index)
{
	return m_textures[index];
}

Sampler* Material::GetSampler(const uint index)
{
	return m_samplers[index];
}

void Material::SetProperty(const char* name, float value)
{
	RemoveProperty(name);
	MaterialPropertyFloat* prop = new MaterialPropertyFloat(name, value); 
	m_properties.push_back(prop);
}

void Material::SetProperty(const char * name, const Vector2& value)
{
	RemoveProperty(name);
	MaterialPropertyVector2* prop = new MaterialPropertyVector2(name, value); 
	m_properties.push_back(prop);
}

void Material::SetProperty(const char * name, const Vector3& value)
{
	RemoveProperty(name);
	MaterialPropertyVector3* prop = new MaterialPropertyVector3(name, value); 
	m_properties.push_back(prop);
}

void Material::SetProperty(const char* name, const Vector4& value)
{
	RemoveProperty(name);
	MaterialPropertyVector4* prop = new MaterialPropertyVector4(name, value); 
	m_properties.push_back(prop);
}

void Material::SetProperty(const char* name, const Rgba& value)
{
	Vector4 out;
	value.GetAsFloats(out.x, out.y, out.z, out.w);
	SetProperty(name, out);
}

void Material::SetTint(const Rgba& tint)
{
	SetProperty("TINT", tint);
}

void Material::SetSpecular(float amount, float power)
{
	RemoveProperty("SPECULAR");
	MaterialPropertySpecular* prop = new MaterialPropertySpecular("SPECULAR", amount, power); 
	m_properties.push_back(prop);
}

void Material::RemoveProperty(const char* name)
{
	for (uint i = 0; i < m_properties.size(); i++)
	{
		MaterialProperty* property = m_properties[i];
		if (property->m_name.compare(name) == 0)
		{
			delete property;

			size_t size = m_properties.size();
			m_properties[i] = m_properties[size - 1];
			m_properties.pop_back();
			i--;

			return; //should never have more than one duplicate in the first place
		}
	}
}

Material* Material::CreateInstance(const std::string& name, Shader* shader)
{
	Material* newMat = new Material(shader);
	newMat->m_name = name;
	return newMat;
}

Material* Material::GetOrCreate(const std::string& path)
{
	std::map<std::string, Material*>::iterator search = g_materialMap.find(path);
	if (search != g_materialMap.end())
	{
		return search->second;
	}
	else
	{
		Material* mat = new Material(path);
		mat->m_is_resource = true;
		g_materialMap.insert(std::pair<std::string, Material*>(path, mat));
		return mat;
	}
}
