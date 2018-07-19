#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Renderer/Material/Material.hpp"

Shader::~Shader()
{
	if (m_programCreated != nullptr)
		delete m_programCreated;
}

Shader::Shader(const std::string & path)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(path.c_str());

	tinyxml2::XMLElement* elementToUse = doc.RootElement();

	m_name = ParseXmlAttribute(*elementToUse, "name", m_name);
	m_sort = ParseXmlAttribute(*elementToUse, "sort", m_sort);
	m_renderQueue = StringToRenderQueue(ParseXmlAttribute(*elementToUse, "queue", "opaque"));
	m_state.m_cullMode = StringToCullMode(ParseXmlAttribute(*elementToUse, "cull", "back"));
	m_state.m_fillMode = StringToFillMode(ParseXmlAttribute(*elementToUse, "fill", "solid"));
	m_state.m_frontFace = StringToWindOrder(ParseXmlAttribute(*elementToUse, "frontFace", "ccw"));

	elementToUse = doc.RootElement()->FirstChildElement();
	while (elementToUse != nullptr)
	{
		std::string elementName = elementToUse->Name();

		if (elementName == "program")
		{
			m_program = new ShaderProgram();
			m_program->m_id  = ParseXmlAttribute(*elementToUse, "id", m_program->m_id);
			std::string defines = ParseXmlAttribute(*elementToUse, "define", "");
			std::string vs_path, fs_path;
			
			tinyxml2::XMLElement* cache = elementToUse;
			elementToUse = elementToUse->FirstChildElement();
			while (elementToUse != nullptr)
			{
				elementName = elementToUse->Name();
				if (elementName == "vertex")
					vs_path = ParseXmlAttribute(*elementToUse, "file", "");
				else if (elementName == "fragment")
					fs_path = ParseXmlAttribute(*elementToUse, "file", "");

				elementToUse = elementToUse->NextSiblingElement();
			}
			elementToUse = cache;

			m_program->LoadFromFiles(vs_path, fs_path, defines.c_str());
			m_programCreated = m_program;
		}
		else if (elementName == "blend")
		{
			tinyxml2::XMLElement* cache = elementToUse;
			elementToUse = elementToUse->FirstChildElement();
			while (elementToUse != nullptr)
			{
				elementName = elementToUse->Name();
				if (elementName == "color")
				{
					m_state.m_colorBlendOp = StringToBlendOp(ParseXmlAttribute(*elementToUse, "op", "add"));
					m_state.m_colorSrcFactor = StringToBlendFactor(ParseXmlAttribute(*elementToUse, "src", "src_alpha"));
					m_state.m_colorDstFactor = StringToBlendFactor(ParseXmlAttribute(*elementToUse, "dest", "inv_src_alpha"));
				}
				else if (elementName == "alpha")
				{
					m_state.m_alphaBlendOp = StringToBlendOp(ParseXmlAttribute(*elementToUse, "op", "add"));
					m_state.m_alphaSrcFactor = StringToBlendFactor(ParseXmlAttribute(*elementToUse, "src", "one"));
					m_state.m_alphaDstFactor = StringToBlendFactor(ParseXmlAttribute(*elementToUse, "dest", "zero"));
				}

				elementToUse = elementToUse->NextSiblingElement();
			}
			elementToUse = cache;
		}
		else if (elementName == "depth")
		{
			m_state.m_depthWrite = ParseXmlAttribute(*elementToUse, "write", true);
			m_state.m_depthCompare = StringToCompareType(ParseXmlAttribute(*elementToUse, "test", "less"));
		}

		elementToUse = elementToUse->NextSiblingElement();
	}

	//load default material
	elementToUse = doc.RootElement();
	m_defaultMaterial = new Material(this);
	m_defaultMaterial->LoadFromXML(*elementToUse);
}

Shader* Shader::Clone()
{
	//deep copy
	Shader* newShader = new Shader();
	newShader->SetProgram(m_program);
	newShader->m_name = m_name;
	newShader->m_sort = m_sort;
	newShader->m_renderQueue = m_renderQueue;
	newShader->m_state = m_state;

	return newShader;
}

void Shader::SetProgram(ShaderProgram* shaderProgram)
{
	m_program = shaderProgram;
}

void Shader::SetColorBlend(eBlendOp op, eBlendFactor src, eBlendFactor dst)
{
	m_state.m_colorBlendOp = op;
	m_state.m_colorSrcFactor = src;
	m_state.m_colorDstFactor = dst;
}

void Shader::SetAlphaBlend(eBlendOp op, eBlendFactor src, eBlendFactor dst)
{
	m_state.m_alphaBlendOp = op;
	m_state.m_alphaSrcFactor = src;
	m_state.m_alphaDstFactor = dst;
}

void Shader::SetDepth(eCompare compare, bool write)
{
	m_state.m_depthCompare = compare;
	m_state.m_depthWrite = write;
}

void Shader::SetCullMode(eCullMode cullMode)
{
	m_state.m_cullMode = cullMode;
}

void Shader::SetFillMode(eFillMode fillMode)
{
	m_state.m_fillMode = fillMode;
}

void Shader::SetFrontFace(eWindOrder windOrder)
{
	m_state.m_frontFace = windOrder;
}
