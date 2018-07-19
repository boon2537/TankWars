#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

eCompare StringToCompareType(const std::string& text)
{
	if (text == "never")	return COMPARE_NEVER;
	if (text == "less")		return COMPARE_LESS;
	if (text == "lequal")	return COMPARE_LEQUAL;
	if (text == "greater")	return COMPARE_GREATER;
	if (text == "gequal")	return COMPARE_GEQUAL;
	if (text == "equal")	return COMPARE_EQUAL;
	if (text == "not")		return COMPARE_NOT_EQUAL;
	if (text == "always")	return COMPARE_ALWAYS;

	ERROR_RECOVERABLE("Compare Type " + text + " not found!");
	return COMPARE_NEVER;
}

eCullMode StringToCullMode(const std::string& text)
{
	if (text == "back")		return CULLMODE_BACK;
	if (text == "front")	return CULLMODE_FRONT;
	if (text == "none")		return CULLMODE_NONE;

	ERROR_RECOVERABLE("Cull mode " + text + " not found!");
	return eCullMode();
}

eFillMode StringToFillMode(const std::string& text)
{
	if (text == "solid")	return FILLMODE_SOLID;
	if (text == "wire")		return FILLMODE_WIRE;

	ERROR_RECOVERABLE("Fill mode " + text + " not found!");
	return eFillMode();
}

eWindOrder StringToWindOrder(const std::string& text)
{
	if (text == "ccw")	return WIND_COUNTER_CLOCKWISE;
	if (text == "cw")	return WIND_CLOCKWISE;

	ERROR_RECOVERABLE("WindOrder " + text + " not found!");
	return eWindOrder();
}

eBlendOp StringToBlendOp(const std::string& text)
{
	if (text == "add")		return OP_ADD;
	if (text == "sub")		return OP_SUB;
	if (text == "rev_sub")	return OP_REV_SUB;
	if (text == "min")		return OP_MIN;
	if (text == "max")		return OP_MAX;

	ERROR_RECOVERABLE("BlendOp " + text + " not found!");
	return eBlendOp();
}

eBlendFactor StringToBlendFactor(const std::string& text)
{
	if (text == "zero")					return BLEND_ZERO;
	if (text == "one")					return BLEND_ONE;
	if (text == "src_color")			return BLEND_SRC_COLOR;
	if (text == "inv_src_color")		return BLEND_ONE_MINUS_SRC_COLOR;
	if (text == "dest_color")			return BLEND_DST_COLOR;
	if (text == "inv_dest_color")		return BLEND_ONE_MINUS_DST_COLOR;
	if (text == "src_alpha")			return BLEND_SRC_ALPHA;
	if (text == "inv_src_alpha")		return BLEND_ONE_MINUS_SRC_ALPHA;
	if (text == "dest_alpha")			return BLEND_DST_ALPHA;
	if (text == "inv_dest_alpha")		return BLEND_ONE_MINUS_DST_ALPHA;
	if (text == "constant_color")		return BLEND_CONSTANT_COLOR;
	if (text == "inv_constant_color")	return BLEND_ONE_MINUS_CONSTANT_COLOR;
	if (text == "constant_alpha")		return BLEND_CONSTANT_ALPHA;
	if (text == "inv_constant_alpha")	return BLEND_ONE_MINUS_CONSTANT_ALPHA;
	if (text == "src_alpha_sat")		return BLEND_SRC_ALPHA_SATURATE;
	if (text == "src1_color")			return BLEND_SRC1_COLOR;
	if (text == "inv_src1_color")		return BLEND_ONE_MINUS_SRC1_COLOR;
	if (text == "src1_alpha")			return BLEND_SRC1_ALPHA;
	if (text == "inv_src1_alpha")		return BLEND_ONE_MINUS_SRC1_ALPHA;

	ERROR_RECOVERABLE("BlendFactor " + text + " not found!");
	return eBlendFactor();
}

eRenderQueue StringToRenderQueue(const std::string & text)
{
	if (text == "alpha")		return RENDER_QUEUE_ALPHA;
	if (text == "opaque")		return RENDER_QUEUE_OPAQUE;

	ERROR_RECOVERABLE("RenderQueue " + text + " not found!");
	return eRenderQueue();
}
