#pragma once
#include <string>

enum eDrawPrimitive
{
	LINES,
	LINE_LOOP,
	TRIANGLES,
	QUADS
};

enum eTextDrawMode
{
	SHRINK_TO_FIT,
	WORD_WRAP,
	OVERRUN
};

enum eCompare
{
	COMPARE_NEVER,       // GL_NEVER
	COMPARE_LESS,        // GL_LESS
	COMPARE_LEQUAL,      // GL_LEQUAL
	COMPARE_GREATER,     // GL_GREATER
	COMPARE_GEQUAL,      // GL_GEQUAL
	COMPARE_EQUAL,       // GL_EQUAL
	COMPARE_NOT_EQUAL,   // GL_NOTEQUAL
	COMPARE_ALWAYS,      // GL_ALWAYS
};
eCompare StringToCompareType(const std::string& text);

enum eRenderDataType
{
	RDT_FLOAT,           // GL_FLOAT
	RDT_UNSIGNED_BYTE,   // GL_UNSIGNED_BYTE
	RDT_TRUE,			 // GL_TRUE
	RDT_FALSE			 // GL_FALSE
						 // ... add more as you need them
};

enum eCullMode 
{
	CULLMODE_BACK,          // GL_BACK     glEnable(GL_CULL_FACE); glCullFace(GL_BACK); 
	CULLMODE_FRONT,         // GL_FRONT    glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); 
	CULLMODE_NONE,          // GL_NONE     glDisable(GL_CULL_FACE)
};
eCullMode StringToCullMode(const std::string& text);

enum eFillMode 
{
	FILLMODE_SOLID,         // GL_FILL     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL )
	FILLMODE_WIRE,          // GL_LINE     glPolygonMode( GL_FRONT_AND_BACK, GL_LINE )
};
eFillMode StringToFillMode(const std::string& text);

enum eWindOrder 
{
	WIND_CLOCKWISE,         // GL_CW       glFrontFace( GL_CW ); 
	WIND_COUNTER_CLOCKWISE, // GL_CCW      glFrontFace( GL_CCW ); 
};
eWindOrder StringToWindOrder(const std::string& text);

enum eBlendOp
{
	OP_ADD,
	OP_SUB,
	OP_REV_SUB,
	OP_MIN,
	OP_MAX
};
eBlendOp StringToBlendOp(const std::string& text);

enum eBlendFactor
{
	BLEND_ZERO,						//0 0 0 0
	BLEND_ONE,						//1 1 1 1
	BLEND_SRC_COLOR,				//R s0 k R G s0 k G B s0 k B A s0 k A
	BLEND_ONE_MINUS_SRC_COLOR,		//1 1 1 1 - R s0 k R G s0 k G B s0 k B A s0 k A
	BLEND_DST_COLOR,				//R d k R G d k G B d k B A d k A
	BLEND_ONE_MINUS_DST_COLOR, 		//1 1 1 1 - R d k R G d k G B d k B A d k A
	BLEND_SRC_ALPHA, 				//A s0 k A A s0 k A A s0 k A A s0 k A
	BLEND_ONE_MINUS_SRC_ALPHA, 		//1 1 1 1 - A s0 k A A s0 k A A s0 k A A s0 k A
	BLEND_DST_ALPHA, 				//A d k A A d k A A d k A A d k A
	BLEND_ONE_MINUS_DST_ALPHA, 		//1 1 1 1 - A d k A A d k A A d k A A d k A
	BLEND_CONSTANT_COLOR, 			//R c G c B c A c
	BLEND_ONE_MINUS_CONSTANT_COLOR,	//1 1 1 1 - R c G c B c A c
	BLEND_CONSTANT_ALPHA, 			//A c A c A c A c
	BLEND_ONE_MINUS_CONSTANT_ALPHA, //1 1 1 1 - A c A c A c A c
	BLEND_SRC_ALPHA_SATURATE, 		//i i i 1
	BLEND_SRC1_COLOR, 				//R s1 k R G s1 k G B s1 k B A s1 k A
	BLEND_ONE_MINUS_SRC1_COLOR, 	//1 1 1 1 - R s1 k R G s1 k G B s1 k B A s1 k A
	BLEND_SRC1_ALPHA, 				//A s1 k A A s1 k A A s1 k A A s1 k A
	BLEND_ONE_MINUS_SRC1_ALPHA, 	//1 1 1 1 - A s1 k A A s1 k A A s1 k A A s1 k A
};
eBlendFactor StringToBlendFactor(const std::string& text);

enum eRenderQueue
{
	RENDER_QUEUE_OPAQUE, 
	RENDER_QUEUE_ALPHA,
	// possibly more needed - add as you see use
};
eRenderQueue StringToRenderQueue(const std::string& text);

enum eTextureCubeSide
{
	TEXCUBE_RIGHT, 
	TEXCUBE_LEFT,  
	TEXCUBE_TOP,   
	TEXCUBE_BOTTOM,
	TEXCUBE_FRONT, 
	TEXCUBE_BACK,
};