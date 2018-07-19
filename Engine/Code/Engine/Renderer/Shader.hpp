#pragma once

#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Material;

struct RenderState_t 
{
	void operator=( const RenderState_t& copyFrom )
	{
		m_cullMode = copyFrom.m_cullMode;
		m_fillMode = copyFrom.m_fillMode;
		m_frontFace = copyFrom.m_frontFace;
		m_depthCompare = copyFrom.m_depthCompare;
		m_depthWrite = copyFrom.m_depthWrite;
		m_colorBlendOp = copyFrom.m_colorBlendOp;
		m_colorSrcFactor = copyFrom.m_colorSrcFactor;
		m_colorDstFactor = copyFrom.m_colorDstFactor;
		m_alphaBlendOp = copyFrom.m_alphaBlendOp;
		m_alphaSrcFactor = copyFrom.m_alphaSrcFactor;
		m_alphaDstFactor = copyFrom.m_alphaDstFactor;
	}

	// Raster State Control
	eCullMode m_cullMode = eCullMode::CULLMODE_NONE; //really should default to BACK
	eFillMode m_fillMode = eFillMode::FILLMODE_SOLID;
	eWindOrder m_frontFace = eWindOrder::WIND_COUNTER_CLOCKWISE;

	// Depth State Control
	eCompare m_depthCompare = eCompare::COMPARE_LESS;
	bool m_depthWrite = true;

	// Blend
	eBlendOp m_colorBlendOp = eBlendOp::OP_ADD;
	eBlendFactor m_colorSrcFactor = eBlendFactor::BLEND_SRC_ALPHA;
	eBlendFactor m_colorDstFactor = eBlendFactor::BLEND_ONE_MINUS_SRC_ALPHA;

	eBlendOp m_alphaBlendOp = eBlendOp::OP_ADD;
	eBlendFactor m_alphaSrcFactor = eBlendFactor::BLEND_ONE;
	eBlendFactor m_alphaDstFactor = eBlendFactor::BLEND_ZERO;
}; 

// Shader is just a combination of a program and render state
class Shader
{
public:
	~Shader();
	Shader() {};
	Shader(const std::string& path);
	Shader* Clone();

	// none of these call GL calls, just sets the internal state
	void SetProgram(ShaderProgram* shaderProgram); 
	
	void SetColorBlend(eBlendOp op, eBlendFactor src, eBlendFactor dst); 
	void EnableColorBlend() { m_state.m_colorSrcFactor = BLEND_SRC_ALPHA; m_state.m_colorDstFactor = BLEND_ONE_MINUS_SRC_ALPHA; };
	void DisableColorBlend() { m_state.m_colorSrcFactor = BLEND_ONE; m_state.m_colorDstFactor = BLEND_ZERO; };
	void SetAlphaBlend(eBlendOp op, eBlendFactor src, eBlendFactor dst); 
	void EnableAlphaBlend() { m_state.m_alphaSrcFactor = BLEND_ONE; m_state.m_alphaDstFactor = BLEND_ONE; };
	void DisableAlphaBlend() { m_state.m_alphaSrcFactor = BLEND_ONE; m_state.m_alphaDstFactor = BLEND_ZERO; };

	void SetDepth(eCompare compare, bool write); 
	void DisableDepth() { SetDepth(COMPARE_ALWAYS, false); }

	void SetCullMode(eCullMode cullMode); 
	void SetFillMode(eFillMode fillMode); 
	void SetFrontFace(eWindOrder windOrder); 

public:
	std::string m_name = "";
	int m_sort = 0; // high level sorting flag
	eRenderQueue m_renderQueue = eRenderQueue::RENDER_QUEUE_OPAQUE; // secondary rendering queue 

	ShaderProgram* m_program = nullptr; 
	RenderState_t m_state; 
	Material* m_defaultMaterial = nullptr;

	bool m_is_resource = false;

private:
	ShaderProgram* m_programCreated = nullptr;
}; 
