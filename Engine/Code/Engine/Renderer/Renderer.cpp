#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/glFunctions.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/ThirdParty/stb_write.h"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/VertexPCU.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Material/MaterialProperty.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Profiler/Profiler.hpp"

#pragma region Built-in Shaders

const BuiltInShaders BuiltInShaders::defaultShader = BuiltInShaders("default",
	//vertex
R"(#version 420 core

layout(binding=0, std140) uniform uboCamera 
{
   mat4 VIEW; 
   mat4 PROJECTION; 
}; 

uniform mat4 MODEL;
uniform vec4 TINT = vec4(1.0, 1.0, 1.0, 1.0);
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;         
out vec2 passUV; 
out vec4 passColor;
void main() 
{
   vec4 local_pos = vec4( POSITION, 1.0f );  
   vec4 world_pos = MODEL * local_pos; 
   vec4 camera_pos = VIEW * world_pos; 
   vec4 clip_pos = PROJECTION * camera_pos; 
   passColor = COLOR * TINT; // pass it on. 
   passUV = UV; 
   gl_Position = clip_pos; 
})",
	//fragment
R"(#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor; // NEW, passed color
in vec2 passUV; 
out vec4 outColor; 
void main() 
{
   vec4 diffuse = texture( gTexDiffuse, passUV );
   outColor = diffuse * passColor;  
})");

const BuiltInShaders BuiltInShaders::errorShader = BuiltInShaders("error",
	//vertex
R"(#version 420 core
in vec3 POSITION;
void main( void )
{
	gl_Position = vec4( POSITION, 1 );
})",
	//fragment
R"(#version 420 core
out vec4 outColor;
void main( void )
{
	outColor = vec4( 1, 0, 1, 1 );
})");

#pragma endregion

void light_t::SetUp(const Vector3& position, bool usesShadow, const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation, const Vector3& direction, 
	float directionFactor, float dotInnerAngle, float dotOuterAngle, const Matrix44& shadowVP)
{
	m_lightPosition = position;

	m_uses_shadow = usesShadow ? 1.f : 0.f;

	float r, g, b, a;
	color.GetAsFloats(r, g, b, a);
	m_lightColor = Vector4(r, g, b, intensity);

	m_attenuation = attenuation;
	m_spec_attunation = spec_attunation;
	m_direction = direction;
	m_directionFactor = directionFactor;
	m_dotInnerAngle = dotInnerAngle;
	m_dotOuterAngle = dotOuterAngle;
	m_shadowVP = shadowVP;
}

static Renderer* g_Renderer = nullptr; 

HMODULE Renderer::gGLLibrary  = NULL; 
HWND Renderer::gGLwnd         = NULL;
HDC Renderer::gHDC            = NULL;
HGLRC Renderer::gGLContext    = NULL;

Renderer::~Renderer()
{

}

Renderer::Renderer()
{

}

// Rendering startup - called after we have created our window
// error checking has been removed for brevity, but you should check
// the return values at each step.
bool Renderer::RenderStartup( HWND hwnd ) 
{
	// load and get a handle to the opengl dll (dynamic link library)
	gGLLibrary = ::LoadLibraryA("opengl32.dll"); 

	// Get the Device Context (DC) - how windows handles the interface to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HDC hdc = ::GetDC(hwnd);       

	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very simiilar to SD1
	HGLRC temp_context = CreateOldRenderContext(hdc); 

	::wglMakeCurrent(hdc, temp_context); 
	BindNewWGLFunctions();  // find the functions we'll need to create the real context; 

							// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext(hdc, 4, 2); 

	// Set and cleanup
	::wglMakeCurrent(hdc, real_context); 
	::wglDeleteContext(temp_context); 

	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions(); 

	// set the globals
	gGLwnd = hwnd;
	gHDC = hdc; 
	gGLContext = real_context; 

	return true; 
}

void Renderer::PostStartup()
{
	m_immediateMesh = new Mesh();

	// default_vao is a GLuint member variable
	glGenVertexArrays(1, &m_default_vao); 
	glBindVertexArray(m_default_vao); 

	//create built-in shaders
	RegisterBuiltInShaders(BuiltInShaders::defaultShader);
	RegisterBuiltInShaders(BuiltInShaders::errorShader);
	m_defaultShader = new Shader();
	SetShader(nullptr);
	m_defaultShader->m_is_resource = true;

	m_tempRenderBuffer = new RenderBuffer();
	m_defaultSampler = new Sampler();
	m_defaultSampler->Create();
	m_shadowSampler = new Sampler();
	m_shadowSampler->CrateShadowSampler();

	glEnable(GL_DEPTH_TEST); 
	glLineWidth(1.5f);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//load plain white image from program-made data
	Image* plainWhiteImage = new Image((unsigned char*) &Rgba::white, 4, IntVector2(1, 1));
	m_plainWhiteTexture = AddTextureFromImage("white", plainWhiteImage);
	delete plainWhiteImage;
	//load flat normal image from program-made data
	Rgba floatColor = Rgba(129, 128, 255, 255);
	Image* flatNormalImage = new Image((unsigned char*) &floatColor, 4, IntVector2(1, 1));
	AddTextureFromImage("flat", flatNormalImage);
	delete flatNormalImage;

	m_defaultMaterial = new Material(m_defaultShader);
	m_defaultMaterial->SetSampler(0, m_defaultSampler);
	m_defaultMaterial->SetTexture2D(0, m_plainWhiteTexture);
	m_defaultMaterial->m_is_resource = true;

	// the default color and depth should match our output window
	unsigned int window_width = Window::GetWidth(); 
	unsigned int window_height = Window::GetHeight();

	// create our output textures
	m_defaultColorTarget = CreateRenderTarget(window_width, window_height);
	m_defaultDepthTarget = CreateDepthStencilTarget(window_width, window_height);

	// setup the initial camera
	m_defaultCamera = new Camera();
	m_defaultCamera->SetColorTarget(m_defaultColorTarget); 
	m_defaultCamera->SetDepthStencilTarget(m_defaultDepthTarget); 
	m_effectCamera = new Camera();

	// set our default camera to be our current camera
	SetCamera(nullptr); 

	//init UBOs
	light_buffer_t temp;
	m_lightBuffer.Set(temp);
	light_object_buffer_t temp1;
	m_lightObjectBuffer.Set(temp1);
	fog_buffer_t temp2;
	m_fogBuffer.Set(temp2);
}

void Renderer::GLShutdown()
{
	wglMakeCurrent( gHDC, NULL ); 

	::wglDeleteContext( gGLContext ); 
	::ReleaseDC( gGLwnd, gHDC ); 

	gGLContext = NULL; 
	gHDC = NULL;
	gGLwnd = NULL; 

	::FreeLibrary( gGLLibrary ); 
}

void Renderer::BeginFrame()
{
	ClearColor(Rgba::black); 
	ClearDepth(1.0f); 
	SetShader(nullptr);
}

void Renderer::EndFrame()
{
	// copies the default camera's frame-buffer to the "null" frame-buffer, 
	// also known as the back buffer.
	CopyFrameBuffer(nullptr, &m_defaultCamera->m_output); 
	SwapBuffers(gHDC); 

	Sleep(1);//to prevent an App from taking 100% of a core
}

Texture* Renderer::CreateOrGetTexture(const std::string& path)
{
	std::map<std::string, Texture*>::iterator search = m_textureMap.find(path);
	if (search != m_textureMap.end())
	{
		return search->second;
	}
	else
	{
		Texture* loadedTexture = new Texture(path);
		m_textureMap.insert(std::pair<std::string, Texture*>(path, loadedTexture));
		return loadedTexture;
	}
}

Texture* Renderer::AddTextureFromImage(const std::string& name, Image* image)
{
	std::map<std::string, Texture*>::iterator search = m_textureMap.find(name);
	if (search != m_textureMap.end())
	{
		return search->second;
	}
	else
	{
		Texture* loadedTexture = new Texture(image);
		m_textureMap.insert(std::pair<std::string, Texture*>(name, loadedTexture));
		return loadedTexture;
	}
}

BitmapFont* Renderer::CreateOrGetBitmapFont(const char * bitmapFontName)
{
	std::map<std::string, BitmapFont*>::iterator search = m_loadedFonts.find(bitmapFontName);
	if (search != m_loadedFonts.end())
	{
		return search->second;
	}
	else
	{
		SpriteSheet* glyphSheet = new SpriteSheet(*CreateOrGetTexture("Data/Fonts/" + std::string(bitmapFontName) + ".png"), 16, 16); //should we clean this up?
		BitmapFont* loadedFont = new BitmapFont(bitmapFontName, *glyphSheet, 1);
		m_loadedFonts.insert(std::pair<std::string, BitmapFont*>(bitmapFontName, loadedFont));
		return loadedFont;
	}
}

Mesh* Renderer::CreateOrGetMesh(const std::string& path)
{
	std::map<std::string, Mesh*>::iterator search = m_loadedMesh.find(path);
	if (search != m_loadedMesh.end())
	{
		return search->second;
	}
	else
	{
		Mesh* loadedMesh = new Mesh();
		MeshBuilder mb;
		mb.AddFromObjFile(path);
		loadedMesh->FromBuilderForType<VertexLit>(mb);
		m_loadedMesh.insert(std::pair<std::string, Mesh*>(path, loadedMesh));
		loadedMesh->m_isResource = true;
		return loadedMesh;
	}
}

ShaderProgram* Renderer::CreateOrGetShaderProgram(const std::string& path, const char* defines)
{
	std::map<std::string, ShaderProgram*>::iterator search = m_loadedShaderPrograms.find(path);
	if (search != m_loadedShaderPrograms.end())
	{
		return search->second;
	}
	else
	{
		//also search in default shaders
		search = m_builtInShaderPrograms.find(path);
		if (search != m_builtInShaderPrograms.end())
		{
			return search->second;
		}
	}

	ShaderProgram* newShaderProgram = new ShaderProgram();

	if (newShaderProgram->LoadFromFiles(path, defines))
	{
		m_loadedShaderPrograms.insert(std::pair<std::string, ShaderProgram*>(path, newShaderProgram));
		return newShaderProgram;
	}
	else
	{
		newShaderProgram = CreateOrGetShaderProgram("error");
		m_loadedShaderPrograms.insert(std::pair<std::string, ShaderProgram*>(path, newShaderProgram));
		return newShaderProgram;
	}
}

ShaderProgram* Renderer::RegisterBuiltInShaders(BuiltInShaders builtInShaders)
{
	std::map<std::string, ShaderProgram*>::iterator search = m_builtInShaderPrograms.find(builtInShaders.m_name);
	if (search != m_builtInShaderPrograms.end())
	{
		return search->second;
	}

	ShaderProgram* newShaderProgram = new ShaderProgram();

	if (newShaderProgram->LoadFromRaw(builtInShaders.m_vertexShader, builtInShaders.m_fragmentShader, builtInShaders.m_defines))
	{
		m_builtInShaderPrograms.insert(std::pair<std::string, ShaderProgram*>(builtInShaders.m_name, newShaderProgram));
		return newShaderProgram;
	}

	ASSERT_RECOVERABLE(false, "failed to register built in shaders");
	return nullptr;
}

void Renderer::ReloadShaders()
{
	std::map<std::string, ShaderProgram*>::iterator it;
	for (it = m_loadedShaderPrograms.begin(); it != m_loadedShaderPrograms.end(); it++)
	{
		ShaderProgram* loadedShaderProgram = it->second;

		bool wasUsingBuiltInErrorShader = loadedShaderProgram == CreateOrGetShaderProgram("error");
		//make sure we don't override the default error shader
		if (wasUsingBuiltInErrorShader)
		{
			loadedShaderProgram = new ShaderProgram();
		}

		if (loadedShaderProgram->LoadFromFiles(it->first, loadedShaderProgram->m_defines))
		{
			it->second = loadedShaderProgram;
		}
		else
		{
			it->second = CreateOrGetShaderProgram("error");
			delete loadedShaderProgram; //delete previous, or recently create to load, shader program
		}
	}
}

void Renderer::SetShaderProgram(ShaderProgram* shaderProgram)
{
	if (shaderProgram == nullptr)
		m_defaultShader->m_program = CreateOrGetShaderProgram("default");
	else
		m_defaultShader->m_program = shaderProgram;

	m_currentShader = m_defaultShader;
	BindShaderProgram(m_defaultShader->m_program);
}

Shader* Renderer::CreateOrGetShader(const std::string& path)
{
	std::map<std::string, Shader*>::iterator search = m_loadedShaders.find(path);
	if (search != m_loadedShaders.end())
	{
		return search->second;
	}
	else
	{
		Shader* shader = new Shader(path);
		shader->m_is_resource = true;
		m_loadedShaders.insert(std::pair<std::string, Shader*>(path, shader));
		return shader;
	}
}

Shader* Renderer::GetShaderByID(const std::string& id)
{
	std::map<std::string, Shader*>::iterator it;
	for (it = m_loadedShaders.begin(); it != m_loadedShaders.end(); it++)
	{
		Shader* potential = it->second;
		if (potential->m_name == id)
		{
			return potential;
		}
	}

	return nullptr;
}

Texture* Renderer::CreateRenderTarget(unsigned int width, unsigned int height, TEXTURE_FORMAT format)
{
	Texture* tex = new Texture();
	tex->CreateRenderTarget(width, height, format);    
	return tex;
}

Texture* Renderer::CreateDepthStencilTarget(unsigned int width, unsigned int height)
{
	return CreateRenderTarget(width, height, TEXTURE_FORMAT_D24S8); 
}

Texture* Renderer::GetDefaultColorTarget()
{
	return m_defaultColorTarget;
}

Texture* Renderer::GetDefaultDepthTarget()
{
	return m_defaultDepthTarget;
}

bool Renderer::CopyFrameBuffer(FrameBuffer* dst, FrameBuffer* src)
{
	// we need at least the src.
	if (src == nullptr) 
		return false; 

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint src_fbo = src->m_handle;
	GLuint dst_fbo = NULL; 
	if (dst != nullptr) 
	{
		dst_fbo = dst->m_handle; 
	}

	// can't copy onto ourselves
	if (dst_fbo == src_fbo)
		return false; 

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src_fbo); 

	// what are we copying to?
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo); 

	// blit it over - get the size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not
	unsigned int width = src->GetWidth();     
	unsigned int height = src->GetHeight(); 

	// Copy it over
	glBlitFramebuffer(0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just color)
		GL_NEAREST);         // resize filtering rule (in case src/dst don't match)

	// Make sure it succeeded
	GL_CHECK_ERROR(); 

	// cleanup after ourselves
	glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL ); 

	return true;
}

void Renderer::EnableDepth(eCompare compare, bool shouldWrite)
{
	m_defaultShader->SetDepth(compare, shouldWrite);
}

void Renderer::DisableDepth()
{
	m_defaultShader->DisableDepth();
}

void Renderer::ClearDepth(float depth)
{
	glDepthMask(true);
	glClearDepthf(depth);
	glClear( GL_DEPTH_BUFFER_BIT ); 
}

void Renderer::ClearColor(const Rgba& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::BindTexture(const uint bindPoint, const Texture* texture)
{
	TODO("find a gl function to check if we have a sampler binded");

	if (texture == nullptr)
		texture = m_plainWhiteTexture;

	glActiveTexture(GL_TEXTURE0 + bindPoint); 
	glBindTexture(GL_TEXTURE_2D, texture->m_handle); 
}

void Renderer::BindCubeMap(const uint bindPoint, const TextureCube* textureCube)
{
	glActiveTexture(GL_TEXTURE0 + bindPoint);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube->GetHandle());
}

void Renderer::BindSampler(const uint bindPoint, Sampler* sampler)
{
	if (sampler == nullptr)
		glBindSampler(bindPoint, m_defaultSampler->GetHandle()); 
	else
		glBindSampler(bindPoint, sampler->GetHandle());
}

void Renderer::BindMeshToProgram(ShaderProgram* program, Mesh* mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.m_handle); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo.m_handle); 

	uint vertex_stride = mesh->GetVertexStride(); 

	GLuint program_handle = program->m_programHandle; 
	uint attrib_count = mesh->m_layout->GetAttributeCount();

	for (uint attrib_idx = 0; attrib_idx < attrib_count; ++attrib_idx) 
	{
		const vertex_attribute_t& attrib = mesh->m_layout->GetAttribute(attrib_idx); 

		// a program needs a name;
		int bind = glGetAttribLocation(program_handle, attrib.name.c_str()); 

		// this attribute exists in this shader, cool, bind it
		if (bind >= 0) 
		{	
			glEnableVertexAttribArray(bind);

			// be sure mesh and program are bound at this point
			// as this links them together
			glVertexAttribPointer( bind,
				attrib.elemCount,      // how many? 
				ToGLType(attrib.type),  // what are they 
				(GLboolean) ToGLType(attrib.isNormalized),      // are they normalized 
				vertex_stride,          // vertex size?
				(GLvoid*) attrib.memberOffset); // data offset from start of vertex
		}
	}
}

void Renderer::SetWireFrameDrawModeActive(bool active)
{
	m_defaultShader->SetFillMode(active ? eFillMode::FILLMODE_WIRE : eFillMode::FILLMODE_SOLID);
}

void Renderer::BindShaderProgram(ShaderProgram* shaderProgram)
{
	glUseProgram(shaderProgram->m_programHandle);
	GL_CHECK_ERROR();
}

void Renderer::BindRenderState(const RenderState_t& state)
{
	//depth
	glDepthFunc(ToGLCompare(state.m_depthCompare));
	glDepthMask(state.m_depthWrite ? GL_TRUE : GL_FALSE);

	//fill mode
	glPolygonMode(GL_FRONT_AND_BACK, ToGLFillMode(state.m_fillMode));

	//cull mode
	if (state.m_cullMode == eCullMode::CULLMODE_NONE)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(TOGLCullMode(state.m_cullMode));
	}

	//wind order
	glFrontFace(ToGLWindOrder(state.m_frontFace)); 

	//blend
	glBlendFuncSeparate(ToGLBlendFactor(state.m_colorSrcFactor), ToGLBlendFactor(state.m_colorDstFactor), ToGLBlendFactor(state.m_alphaSrcFactor), ToGLBlendFactor(state.m_alphaDstFactor));
	glBlendEquationSeparate(ToGLBlendOp(state.m_colorBlendOp), ToGLBlendOp(state.m_alphaBlendOp));
}

void Renderer::SetShader(Shader* shader) 
{
	if (shader == nullptr)
	{
		m_defaultShader->m_program = CreateOrGetShaderProgram("default");
		m_currentShader = m_defaultShader;
		BindShaderProgram(m_currentShader->m_program);
		return;
	}

	m_currentShader = shader;
	BindShaderProgram(m_currentShader->m_program);
}

void Renderer::BindMaterial(Material* mat)
{
	SetShader(mat->GetShader());

	//bind textures
	std::map<uint, Texture*>::iterator it;
	for (it = mat->m_textures.begin(); it !=  mat->m_textures.end(); it++)
	{
		if (it->second != nullptr)
		{
			//if a sampler is not found
			if (mat->m_samplers[it->first] == nullptr)
				BindSampler(it->first, m_defaultSampler);
			else
				BindSampler(it->first, mat->m_samplers[it->first]);

			BindTexture(it->first, it->second);
		}
	}

	//bind UniformBlock

	//bind properties
	GLuint programHandle = m_currentShader->m_program->m_programHandle; 
	for (uint i = 0; i < mat->m_properties.size(); i++)
	{
		MaterialProperty* prop = mat->m_properties[i];
		int bind_point = glGetUniformLocation(programHandle, prop->m_name.c_str());
		if (bind_point > 0 || prop->m_ignoreBindPoint)
		{
			prop->Bind(bind_point);
		}
	}
}

void Renderer::DrawAABB(const AABB2& bounds, const Rgba & color)
{
	DrawTexturedAABB(bounds, *m_plainWhiteTexture, Vector2(0, 1), Vector2(1, 0), color);
}

void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint)
{
	unsigned int textureIndex = 0U; // to see how they tie together

	BindSampler();
	// Bind the texture
	glActiveTexture(GL_TEXTURE0 + textureIndex); 
	glBindTexture(GL_TEXTURE_2D, texture.m_handle); 

	VertexPCU vertexs[6];
	
	vertexs[0] = VertexPCU(Vector2(bounds.mins.x, bounds.mins.y), tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y)); // 0 - (-1, -1)
	vertexs[1] = VertexPCU(Vector2(bounds.maxs.x, bounds.mins.y), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y)); // 1 - (1, -1)
	vertexs[2] = VertexPCU(Vector2(bounds.mins.x, bounds.maxs.y), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y)); // 2 - (-1, 1)

	vertexs[3] = VertexPCU(Vector2(bounds.mins.x, bounds.maxs.y), tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y)); // 2 - (-1, 1)
	vertexs[4] = VertexPCU(Vector2(bounds.maxs.x, bounds.mins.y), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y)); // 1 - (1, -1)
	vertexs[5] = VertexPCU(Vector2(bounds.maxs.x, bounds.maxs.y), tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y)); // 3 - (1, 1)

	DrawMeshImmediate(vertexs, 6, eDrawPrimitive::TRIANGLES);
}

void Renderer::DrawTexturedAABB2_3D(const Vector3& position, const Vector2& dimensions, const Vector2& pivot, const Vector2& scale, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Matrix44& orientation, const Rgba& tint)
{
	float left = -1 * (pivot.x * dimensions.x * scale.x);
	float right = left + (dimensions.x * scale.x);
	float bottom = -1 * (pivot.y * dimensions.y * scale.y);
	float top = bottom + (dimensions.y * scale.y);

	//using model matrix
	Vector3 right_vec = orientation.GetRight();
	Vector3 up_vec = orientation.GetUp();
	Vector3 p0 = position + (right_vec * left) + (up_vec * bottom);
	Vector3 p1 = position + (right_vec * right) + (up_vec * bottom);
	Vector3 p2 = position + (right_vec * left) + (up_vec * top);
	Vector3 p3 = position + (right_vec * right) + (up_vec * top);

	unsigned int textureIndex = 0U; // to see how they tie together

	BindSampler();
	// Bind the texture
	glActiveTexture(GL_TEXTURE0 + textureIndex); 
	glBindTexture(GL_TEXTURE_2D, texture.m_handle); 

	// 2---3
	// |   |
	// 0---1
	VertexPCU vertexs[6];

	vertexs[0] = VertexPCU(p0, tint, Vector2(texCoordsAtMins.x, texCoordsAtMins.y));
	vertexs[1] = VertexPCU(p1, tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertexs[2] = VertexPCU(p2, tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));

	vertexs[3] = VertexPCU(p2, tint, Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y));
	vertexs[4] = VertexPCU(p1, tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y));
	vertexs[5] = VertexPCU(p3, tint, Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y));

	DrawMeshImmediate(vertexs, 6, eDrawPrimitive::TRIANGLES);
}

void Renderer::DrawTextureFromVertexArray(const VertexPCU* verts, int numVerts, const Texture& texture)
{
	unsigned int textureIndex = 0U; // to see how they tie together
	// Bind the sampler
	glBindSampler(textureIndex, m_defaultSampler->GetHandle()); 
	// Bind the texture
	glActiveTexture(GL_TEXTURE0 + textureIndex); 
	glBindTexture(GL_TEXTURE_2D, texture.m_handle); 

	DrawMeshImmediate(verts, numVerts, eDrawPrimitive::TRIANGLES);
}

void Renderer::DrawSprite(const Vector3& position, Sprite* sprite, const Matrix44& orientation, const Vector2& scale, const Rgba& tint)
{
	TODO("factor in sprite's base tint");
	AABB2 uvs = sprite->GetNormalizedUV();
	DrawTexturedAABB2_3D(position, sprite->m_dimensions, sprite->m_pivot, scale * sprite->m_scale, *sprite->m_texture, uvs.mins, uvs.maxs, orientation, tint);
}

void Renderer::DrawPoly(int numSides, float orientation, float radius, const Vector2& center, const Rgba& color)
{
	float degreesPerVertex = 360.f / (float) numSides;
	for (float i = 0; i < numSides; i++)
	{
		float startDegrees = orientation + degreesPerVertex * i;
		float endDegrees = orientation + degreesPerVertex * (i + 1);
		float startX = center.x + (radius * CosDegrees(startDegrees));
		float startY = center.y + (radius * SinDegrees(startDegrees));
		float endX = center.x + (radius * CosDegrees(endDegrees));
		float endY = center.y + (radius * SinDegrees(endDegrees));

		DrawLine(Vector2(startX, startY), Vector2(endX, endY), color);
	}
}

void Renderer::DrawFromVertexArray(const Vector2 vertexArray[], int arraySize, const Vector2& center, float orientation, float scale, const Rgba& color)
{
	PushMatrix();
	Translate2DInMatrix(center);
	Rotate2DInMatrix(orientation);
	UniformScale2DInMatrix(scale);

	for (int i = 0; i < arraySize - 1; i++)
		DrawLine(Vector2(vertexArray[i].x, vertexArray[i].y), Vector2(vertexArray[i + 1].x, vertexArray[i + 1].y), color);

	PopMatrix();
}

void Renderer::DrawDashedCircle(const Vector2& center, float radius, const Rgba& color)
{
	float degreesPerVertex = 360.f / 30.f;
	for (int i = 0; i < 30; i++)
	{
		if (i % 2 == 0)
		{
			float startDegrees = degreesPerVertex * i;
			float endDegrees = degreesPerVertex * (i + 1);
			float startX = center.x + (radius * CosDegrees(startDegrees));
			float startY = center.y + (radius * SinDegrees(startDegrees));
			float endX = center.x + (radius * CosDegrees(endDegrees));
			float endY = center.y + (radius * SinDegrees(endDegrees));

			DrawLine(Vector2(startX, startY), Vector2(endX, endY), color);
		}
	}
}

void Renderer::DrawLine(const Vector2& p1, const Vector2& p2, const Rgba& color)
{
	DrawLine(p1, p2, color, color);
}

void Renderer::DrawLine(const Vector2 & p1, const Vector2 & p2, const Rgba & colorBegin, const Rgba & colorEnd)
{
	VertexPCU vertexs[2];
	vertexs[0] = VertexPCU(p1, colorBegin);
	vertexs[1] = VertexPCU(p2, colorEnd);

	BindSampler();
	BindTexture();
	DrawMeshImmediate(vertexs, 2, eDrawPrimitive::LINES);
}

void Renderer::DrawLine(const Vector3 & p1, const Vector3 & p2, const Rgba & color)
{
	DrawLine(p1, p2, color, color);
}

void Renderer::DrawLine(const Vector3 & p1, const Vector3 & p2, const Rgba & colorBegin, const Rgba & colorEnd)
{
	VertexPCU vertexs[2];
	vertexs[0] = VertexPCU(p1, colorBegin);
	vertexs[1] = VertexPCU(p2, colorEnd);

	BindSampler();
	BindTexture();
	DrawMeshImmediate(vertexs, 2, eDrawPrimitive::LINES);
}

#pragma region Text Drawing

void Renderer::DrawText2D(const Vector2 & drawMins, const std::string & asciiText, float cellHeight, const Rgba & tint, float aspectScale, const BitmapFont * font)
{
	PROFILE_SCOPE_FUNCTION();

	const BitmapFont* fontToUse = font;
	//use fist font we have as a fall back (if we have any)
	if (fontToUse == nullptr)
	{
		if (m_loadedFonts.size() == 0)
			return;

		fontToUse = m_loadedFonts.begin()->second;
	}

	float cellWidth = cellHeight * aspectScale; //TODO: make cellWidth dynamic when we upgrade from Bitmap font
	float anchorX = drawMins.x;
	float anchorY = drawMins.y;
	AABB2 characterUV;

	MeshBuilder mb;

	for (unsigned int character = 0; character < asciiText.length(); character++)
	{
		characterUV = fontToUse->GetUVsForGlyph(asciiText[character]);
		mb.AddSprite(Vector3(AABB2(anchorX, anchorY, anchorX  + cellWidth, anchorY + cellHeight).GetCenter(), 0.f), Vector2(cellWidth, cellHeight), Vector3::one, tint, characterUV);
 		anchorX += cellWidth;
	}

	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexPCU>(mb);
	BindSampler();
	BindTexture(0, &fontToUse->m_spriteSheet.GetTexture());
	DrawMesh(mesh);
	delete mesh;
}

void Renderer::DrawTextInBox2D(const AABB2 & bounds, const std::string & asciiText, float cellHeight, const Rgba & tint, float aspectScale, const BitmapFont * font, eTextDrawMode textDrawMode, const Vector2 & alignment)
{
	PROFILE_SCOPE_FUNCTION();

	float scale = 1;
	float leftPadding = 0;
	float topPadding = 0;
	float boxHeight = bounds.GetDimensions().y;
	float boxWidth = bounds.GetDimensions().x;
	float cellHeightWithScale = cellHeight;
	float cellWidthWithScale = cellHeightWithScale * aspectScale; //TODO: make cellWidth dynamic when we upgrade from Bitmap font
	Strings textList = Split(asciiText, '\n');
	float textHeight = cellHeightWithScale * textList.size();

	//DEBUG draw box
	//DrawAABB(bounds, Rgba::cyan);

	if (textDrawMode == eTextDrawMode::SHRINK_TO_FIT)
	{
		float textMaxWidth = 0;
		for (unsigned int line = 0; line < textList.size(); line++)
		{
			float lineWidth = textList[line].size() * cellWidthWithScale;
			if (lineWidth > textMaxWidth)
				textMaxWidth = lineWidth;
		}

		//determine exact scale to shrink
		float scaleX = 1;
		float scaleY = 1;
		if (textHeight > boxHeight)
			scaleY = boxHeight / textHeight;
		if (textMaxWidth > boxWidth)
			scaleX = boxWidth / textMaxWidth;
		scale = min(scaleX, scaleY);

		//apply scale
		cellHeightWithScale *= scale;
		cellWidthWithScale *= scale;
		textHeight = cellHeightWithScale * textList.size();
	}

	if (textDrawMode == eTextDrawMode::WORD_WRAP)
	{
		Strings wrappedTextList;
		for (unsigned int line = 0; line < textList.size(); line++)
		{
			float lineWidth = textList[line].size() * cellWidthWithScale;
			if (lineWidth > boxWidth)
			{
				Strings choppedLine = Split(textList[line], ' ');

				//in case there is no way to split a line
				if (choppedLine.size() <= 1)
				{
					wrappedTextList.push_back(textList[line]);
					continue;
				}

				while (choppedLine.size() > 0)
				{
					std::string newLine = choppedLine[0];
					for (unsigned int index = 0; index < choppedLine.size(); index++)
					{
						// there is only one word left, forced to use it; there is no next word; when you add a new word and a space (hence +1), you will overflow
						if (choppedLine.size() == 1 || index + 1 == choppedLine.size() || (newLine.size() + choppedLine[index + 1].size() + 1) * cellWidthWithScale > boxWidth)
						{
							wrappedTextList.push_back(newLine);
							choppedLine.erase(choppedLine.begin(), choppedLine.begin() + index + 1);
							break;
						}
						// keep adding a word otherwise
						newLine = newLine + " " + choppedLine[index + 1];
					}
				}
			}
			else
				wrappedTextList.push_back(textList[line]);
		}

		//replace text list used to draw
		textList = wrappedTextList;

		//scale down
		textHeight = cellHeightWithScale * textList.size();
		if (textHeight > boxHeight)
		{
			scale = boxHeight / textHeight;
			cellHeightWithScale *= scale;
			cellWidthWithScale *= scale;
			textHeight = cellHeightWithScale * textList.size();
		}
	}

	topPadding = (boxHeight - textHeight) * alignment.y;
	float startY = bounds.mins.y + boxHeight - topPadding - cellHeightWithScale;
	for (unsigned int line = 0; line < textList.size(); line++)
	{
		leftPadding = (boxWidth - (textList[line].size() * cellWidthWithScale)) * alignment.x;
		float startX = bounds.mins.x + leftPadding;
		DrawText2D(Vector2(startX, startY), textList[line], cellHeightWithScale, tint, aspectScale, font);
		startY -= cellHeightWithScale;
	}
}

void Renderer::DrawTextInBox3D(const Vector3& position, const AABB2& bounds, const Matrix44& orientation, const std::string& asciiText, float cellHeight, const Vector2& alignment, const Rgba& tint, eTextDrawMode textDrawMode, float aspectScale, const BitmapFont * font)
{
	float scale = 1;
	float leftPadding = 0;
	float topPadding = 0;
	float boxHeight = bounds.GetDimensions().y;
	float boxWidth = bounds.GetDimensions().x;
	float cellHeightWithScale = cellHeight;
	float cellWidthWithScale = cellHeightWithScale * aspectScale; //TODO: make cellWidth dynamic when we upgrade from Bitmap font
	Strings textList = Split(asciiText, '\n');
	float textHeight = cellHeightWithScale * textList.size();

	//DEBUG draw box
	//DrawAABB(bounds, Rgba::cyan);

	if (textDrawMode == eTextDrawMode::SHRINK_TO_FIT)
	{
		float textMaxWidth = 0;
		for (unsigned int line = 0; line < textList.size(); line++)
		{
			float lineWidth = textList[line].size() * cellWidthWithScale;
			if (lineWidth > textMaxWidth)
				textMaxWidth = lineWidth;
		}

		//determine exact scale to shrink
		float scaleX = 1;
		float scaleY = 1;
		if (textHeight > boxHeight)
			scaleY = boxHeight / textHeight;
		if (textMaxWidth > boxWidth)
			scaleX = boxWidth / textMaxWidth;
		scale = min(scaleX, scaleY);

		//apply scale
		cellHeightWithScale *= scale;
		cellWidthWithScale *= scale;
		textHeight = cellHeightWithScale * textList.size();
	}

	if (textDrawMode == eTextDrawMode::WORD_WRAP)
	{
		Strings wrappedTextList;
		for (unsigned int line = 0; line < textList.size(); line++)
		{
			float lineWidth = textList[line].size() * cellWidthWithScale;
			if (lineWidth > boxWidth)
			{
				Strings choppedLine = Split(textList[line], ' ');

				//in case there is no way to split a line
				if (choppedLine.size() <= 1)
				{
					wrappedTextList.push_back(textList[line]);
					continue;
				}

				while (choppedLine.size() > 0)
				{
					std::string newLine = choppedLine[0];
					for (unsigned int index = 0; index < choppedLine.size(); index++)
					{
						// there is only one word left, forced to use it; there is no next word; when you add a new word and a space (hence +1), you will overflow
						if (choppedLine.size() == 1 || index + 1 == choppedLine.size() || (newLine.size() + choppedLine[index + 1].size() + 1) * cellWidthWithScale > boxWidth)
						{
							wrappedTextList.push_back(newLine);
							choppedLine.erase(choppedLine.begin(), choppedLine.begin() + index + 1);
							break;
						}
						// keep adding a word otherwise
						newLine = newLine + " " + choppedLine[index + 1];
					}
				}
			}
			else
				wrappedTextList.push_back(textList[line]);
		}

		//replace text list used to draw
		textList = wrappedTextList;

		//scale down
		textHeight = cellHeightWithScale * textList.size();
		if (textHeight > boxHeight)
		{
			scale = boxHeight / textHeight;
			cellHeightWithScale *= scale;
			cellWidthWithScale *= scale;
			textHeight = cellHeightWithScale * textList.size();
		}
	}

	Vector3 right_vec = orientation.GetRight();
	Vector3 up_vec = orientation.GetUp();

	topPadding = (boxHeight - textHeight) * alignment.y;
	float startY = bounds.mins.y + boxHeight - topPadding - cellHeightWithScale;
	for (unsigned int line = 0; line < textList.size(); line++)
	{
		leftPadding = (boxWidth - (textList[line].size() * cellWidthWithScale)) * alignment.x;
		float startX = bounds.mins.x + leftPadding;
		Vector3 drawMins = position + (right_vec * startX) + (up_vec * startY);
		DrawText3D(drawMins, textList[line], cellHeightWithScale, orientation, tint, font, aspectScale);
		startY -= cellHeightWithScale;
	}
}

void Renderer::DrawText3D(const Vector3& botLeftText, std::string& asciiText, float cellHeight, const Matrix44& orientation, const Rgba& tint, const BitmapFont* font, float aspectScale)
{
	const BitmapFont* fontToUse = font;
	//use fist font we have as a fall back (if we have any)
	if (fontToUse == nullptr)
	{
		if (m_loadedFonts.size() == 0)
			return;

		fontToUse = m_loadedFonts.begin()->second;
	}

	float cellWidth = cellHeight * aspectScale; //TODO: make cellWidth dynamic when we upgrade from Bitmap font
	Vector2 dimensions = Vector2(cellWidth, cellHeight);
	Vector3 anchorPos = botLeftText;
	AABB2 characterUV;

	for (unsigned int character = 0; character < asciiText.length(); character++)
	{
		characterUV = fontToUse->GetUVsForGlyph(asciiText[character]);

		float left = 0;
		float right = left + cellWidth;
		float bottom = 0;
		float top = bottom + cellHeight;
		// 2---3
		// |   |
		// 0---1
		//using model matrix
		Vector3 right_vec = orientation.GetRight();
		Vector3 up_vec = orientation.GetUp();
		Vector3 p0 = anchorPos + (right_vec * left) + (up_vec * bottom);
		Vector3 p1 = anchorPos + (right_vec * right) + (up_vec * bottom);
		Vector3 p2 = anchorPos + (right_vec * left) + (up_vec * top);
		Vector3 p3 = anchorPos + (right_vec * right) + (up_vec * top);
		Vector3 center = (p0 + p1 + p2 + p3) / 4.f;

		DrawTexturedAABB2_3D(center, Vector2(cellWidth, cellHeight), Vector2(0.5f, 0.5f), Vector2(1, 1), fontToUse->m_spriteSheet.GetTexture(), Vector2(characterUV.mins.x, characterUV.maxs.y), Vector2(characterUV.maxs.x, characterUV.mins.y), orientation, tint);
		anchorPos = p1;
	}
}

#pragma endregion

void Renderer::DrawCube(const Vector3& center, const Vector3& dimensions, const Rgba& tint, const AABB2& uv_top, const AABB2& uv_side, const AABB2& uv_bottom)
{
	DrawTexturedCube(center, dimensions, m_plainWhiteTexture, tint, uv_top, uv_side, uv_bottom);
}

void Renderer::DrawTexturedCube(const Vector3& center, const Vector3& dimensions, Texture* texture, const Rgba& tint, const AABB2& uv_top, const AABB2& uv_side, const AABB2& uv_bottom)
{
	if (texture == nullptr)
		texture = m_plainWhiteTexture;

	MeshBuilder mb;
	mb.AddCube(center, dimensions, Vector3::one, tint, uv_top, uv_side, uv_bottom);
	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexPCU>(mb);

	BindSampler();
	BindTexture();
	DrawMesh(mesh);
	delete mesh;
}

void Renderer::DrawUVSphere(const Vector3& center, float radius, const Rgba& tint, Texture* texture)
{
	if (texture == nullptr)
		texture = m_plainWhiteTexture;

	MeshBuilder mb;
	mb.AddUVSphere(center, radius, 32, 16, tint);
	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexPCU>(mb);

	BindSampler();
	BindTexture();
	DrawMesh(mesh);
	delete mesh;
}

void Renderer::DrawPlane(const Vector3& center, const Vector3& right, const Vector3& up, const AABB2& bounds, Texture* texture, const Rgba& tint)
{
	if (texture == nullptr)
		texture = m_plainWhiteTexture;

	Matrix44 mat = Matrix44(right, up, Vector3::zero);
	DrawTexturedAABB2_3D(center, bounds.GetDimensions(), Vector2(0.5f, 0.5f), Vector2(1, 1), *texture, Vector2::zero, Vector2(1, 1), mat, tint);
}

void Renderer::DrawMeshImmediate(const VertexPCU* verts, int numVerts, eDrawPrimitive drawPrimitive, uint icount, uint* indices)
{
	m_immediateMesh->SetVertices<VertexPCU>(numVerts, verts);
	m_immediateMesh->SetIndices(icount, indices);

	draw_instruction_t draw; 
	draw.m_primitiveType = drawPrimitive; 
	draw.m_startIndex = 0; 
	draw.m_usingIndices = (icount > 0);
	draw.m_elemCount = draw.m_usingIndices ? icount : numVerts; 
	m_immediateMesh->m_drawCall = draw; 

	DrawMesh(m_immediateMesh, m_matrixStack.GetTop());
}

void Renderer::DrawMesh(Mesh* mesh, const Matrix44& model)
{
	ProfilerPush(__FUNCTION__);

	BindShaderProgram(m_currentShader->m_program); 
	BindRenderState(m_currentShader->m_state); 
	BindMeshToProgram(m_currentShader->m_program, mesh);

	// Tell GL what shader program to use.
	GLuint program_handle = m_currentShader->m_program->m_programHandle; 

	// Next, bind the buffers we want to use; 
	glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo.m_handle); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo.m_handle);

	// Update and bind UBOs
	m_activeCamera->m_cameraBuffer->UpdateGPU();
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_activeCamera->m_cameraBuffer->GetHandle()); 
	m_lightBuffer.UpdateGPU();
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, m_lightBuffer.GetHandle()); 
	m_lightObjectBuffer.UpdateGPU();
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, m_lightObjectBuffer.GetHandle());
	m_fogBuffer.UpdateGPU();
	glBindBufferBase(GL_UNIFORM_BUFFER, 7, m_fogBuffer.GetHandle());

	TODO("put time in UBO");
	SetUniform("GAME_TIME", static_cast<float>(m_gameTimeRef));

	//bind leftover Uniforms
	GLint bind = glGetUniformLocation(program_handle, "MODEL");
	if (bind >= 0) {
		glUniformMatrix4fv(bind, 1, GL_FALSE, (GLfloat*) &model);
	}
	bind = glGetUniformLocation(program_handle, "TIME");
	if (bind >= 0) {
		float time = static_cast<float>(GetSystemCurrentTime());
		glUniform1f(bind, time);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_activeCamera->GetFrameBufferHandle());
	
	if (mesh->m_drawCall.m_usingIndices)
	{
		glDrawElements(
			ToGLPrimitive(mesh->m_drawCall.m_primitiveType),      // mode
			mesh->m_drawCall.m_elemCount,    // count
			GL_UNSIGNED_INT,  
			(void*) 0 // element array buffer offset
		);
	}
	else
	{
		glDrawArrays(ToGLPrimitive(mesh->m_drawCall.m_primitiveType), 0, mesh->m_drawCall.m_elemCount);
	}

	GL_CHECK_ERROR();

	ProfilerPop();
}

void Renderer::DrawMeshWithMaterial(Material* mat, Mesh* mesh, const Matrix44& model)
{
	if (mat == nullptr)
		mat = m_defaultMaterial;

	BindMaterial(mat);
	DrawMesh(mesh, model);
}

void Renderer::SetCamera(Camera* camera)
{
	if (camera == nullptr) 
		camera = m_defaultCamera; 

	camera->Finalize(); // make sure the frame-buffer is finished being setup
	m_activeCamera = camera;

	//set_camera should call glViewport with resolution of render target
	glViewport(0, 0, m_activeCamera->m_output.m_depthStencilTarget->GetWidth(), m_activeCamera->m_output.m_depthStencilTarget->GetHeight());
}

Camera* Renderer::GetActiveCamera()
{
	return m_activeCamera;
}

void Renderer::SetProjectionOrtho(float width, float height, float orthoNear, float orthoFar)
{
	if (m_activeCamera != nullptr)
	{
		m_activeCamera->SetProjectionOrtho(width, height, orthoNear, orthoFar);
	}
}

void Renderer::SetProjectionMatrix(const Matrix44& projection)
{
	if (m_activeCamera != nullptr)
	{
		m_activeCamera->SetProjection(projection);
	}
}

void Renderer::PushMatrix()
{
	m_matrixStack.Push();
}

void Renderer::Translate2DInMatrix(const Vector2& center)
{
	m_matrixStack.Transform(Matrix44::MakeTranslation2D(center));
}

void Renderer::Rotate2DInMatrix(const float orientation)
{
	m_matrixStack.Rotate(Matrix44::MakeRotationDegrees2D(orientation));
}

void Renderer::UniformScale2DInMatrix(const float scale)
{
	m_matrixStack.Scale(Matrix44::MakeScaleUniform2D(scale));
}

void Renderer::PopMatrix()
{
	m_matrixStack.Pop();
}

void Renderer::SetAdditiveBlendingActive(bool active)
{
	if (active)
		m_defaultShader->EnableColorBlend();
	else
		m_defaultShader->DisableColorBlend();
}

void Renderer::SetUniform(const char* name, float f)
{
	GLint bind_idx = glGetUniformLocation(m_currentShader->m_program->m_programHandle, name);
	if (bind_idx >= 0) 
		glUniform1fv(bind_idx, 1, &f);
}

void Renderer::SetUniform(const char* name, const Vector3& v)
{
	GLint bind_idx = glGetUniformLocation(m_currentShader->m_program->m_programHandle, name);
	if (bind_idx >= 0) 
		glUniform3fv(bind_idx, 1, (GLfloat*) &v);
}

void Renderer::SetUniform(const char* name, const Vector4& v)
{
	GLint bind_idx = glGetUniformLocation(m_currentShader->m_program->m_programHandle, name);
	if (bind_idx >= 0) 
		glUniform4fv(bind_idx, 1, (GLfloat*) &v);
}

void Renderer::SetUniform(const char* name, const Rgba& color)
{
	Vector4 out;
	color.GetAsFloats(out.x, out.y, out.z, out.w);
	GLint bind_idx = glGetUniformLocation(m_currentShader->m_program->m_programHandle, name);
	if (bind_idx >= 0) 
		glUniform4fv(bind_idx, 1, (GLfloat*) &out);
}

void Renderer::ApplyEffect(Shader* shader)
{
	if (m_effectTarget == nullptr)
	{
		m_effectTarget = m_defaultColorTarget;
		if (m_effectScratch == nullptr)
		{
			m_effectScratch = Texture::CreateCompatible(m_effectTarget);
		}
	}

	//drawing full screen quad
	m_effectCamera->SetColorTarget(m_effectScratch);
	SetCamera(m_effectCamera);

	SetShader(shader);

	DrawTexturedAABB(AABB2(-1.f, -1.f, 1.f, 1.f), *m_effectTarget, Vector2(0, 1), Vector2(1, 0), Rgba::white);

	std::swap(m_effectTarget, m_effectScratch);
}

void Renderer::FinishEffects()
{
	if (m_effectTarget == nullptr)
		return;

	if (m_effectTarget != m_defaultColorTarget)
	{
		Texture::CopyTexture(m_effectTarget, m_defaultColorTarget);
		m_effectScratch = m_effectTarget;
	}

	m_effectTarget = nullptr;
}

void Renderer::SetAmbientLight(float intensity, const Rgba& color)
{
	light_buffer_t* buff = m_lightBuffer.as<light_buffer_t>();
	float r, g, b, a;
	color.GetAsFloats(r, g, b, a);
	buff->m_ambience = Vector4(r, g, b, intensity);
}

void Renderer::DisableAllLights()
{
	light_buffer_t* buff = m_lightBuffer.as<light_buffer_t>();
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		buff->m_lights[i].m_lightColor.w = 0.f;
	}
}

void Renderer::EnableLight(Light* light, uint index)
{
	light_buffer_t* buff = m_lightBuffer.as<light_buffer_t>();
	buff->m_lights[index].SetUp(light->m_transform.GetWorldPosition(), light->m_usesShadow, light->m_color, light->m_intensity, light->m_attenuation,
		light->m_spec_attunation, light->m_transform.GetWorldMatrix().GetForward(), light->m_directionFactor, light->m_dotInnerAngle, light->m_dotOuterAngle, light->m_shadowVP);

	//bind shadowInfo
	if (light->IsDirectionalLight())
	{
		BindSampler(8, m_shadowSampler);
		BindTexture(8, light->GetorCreateShadowTexture());
	}
}

void Renderer::SetSpecularConstants(float specAmount, float specPower)
{
	light_object_buffer_t* buff = m_lightObjectBuffer.as<light_object_buffer_t>();
	buff->m_specAmount = specAmount;
	buff->m_specPower = specPower;
}

void Renderer::SetFog(const Rgba& color, float nearPlane, float farPlane, float nearFactor, float farFactor)
{
	fog_buffer_t* buff = m_fogBuffer.as<fog_buffer_t>();
	float r, g, b, a;
	color.GetAsFloats(r, g, b, a);

	buff->m_fogColor = Vector4(r, g, b, a);
	buff->m_fogNearPlane = nearPlane;
	buff->m_fogFarPlane = farPlane;
	buff->m_fogNearFactor = nearFactor;
	buff->m_fogFarFactor = farFactor;
}

void Renderer::UpdateTimeBlock(float currentTime)
{
	m_gameTimeRef = currentTime;
}

Texture* Renderer::GetPlainWhiteTexture()
{
	return m_plainWhiteTexture;
}

void Renderer::TakeScreenShot()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int* width  = &viewport[2];
	int* height = &viewport[3];

	Texture* screenShotTexture = m_defaultCamera->m_output.m_colorTarget;
	GLuint texture = screenShotTexture->GetHandle();
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// rgb image
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewport[0],
		viewport[1], viewport[2], viewport[3], 0);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	BYTE* raw_img = (BYTE*) malloc(sizeof(BYTE) * (*width) * (*height) * 3);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_img);

	stbi_flip_vertically_on_write(1);

	std::string fileName = std::string("Screenshots/screenshot_") + GetFormatedDateTime() + std::string(".png");
	int err = stbi_write_png(fileName.c_str(), *width, *height, 3, raw_img, *width * 3);
	if (err == 0)
	{
		ASSERT_RECOVERABLE(false, "Screenshot failed! Create Screenshots folder in Run_Win32");
	}
	else if (err == 1)
	{
		ConsolePrintf("Screenshot taken!");
	}

	stbi_flip_vertically_on_write(0);
}

void Renderer::TextureGetInternalFormat(GLuint* out_internalFormat, GLuint* out_channels, GLuint* out_pixelLayout, GLuint format)
{
	if (format == GL_R8)
	{
		*out_internalFormat = GL_R8;
		*out_channels = GL_RED;
		*out_pixelLayout = GL_UNSIGNED_BYTE;
	}
	else if (format == GL_RG8)
	{
		*out_internalFormat = GL_RG8;
		*out_channels = GL_RG;
		*out_pixelLayout = GL_UNSIGNED_BYTE;
	}
	else if (format == GL_RGB8)
	{
		*out_internalFormat = GL_RGB8;
		*out_channels = GL_RGB;
		*out_pixelLayout = GL_UNSIGNED_BYTE;
	}
	else if (format == GL_RGBA8)
	{
		*out_internalFormat = GL_RGBA8;
		*out_channels = GL_RGBA;
		*out_pixelLayout = GL_UNSIGNED_BYTE;
	}
}

GLenum Renderer::ToGLCompare(eCompare compare)
{
	if (compare == eCompare::COMPARE_NEVER)		return GL_NEVER;
	if (compare == eCompare::COMPARE_LESS)		return GL_LESS;
	if (compare == eCompare::COMPARE_LEQUAL)		return GL_LEQUAL;
	if (compare == eCompare::COMPARE_GREATER)	return GL_GREATER;
	if (compare == eCompare::COMPARE_GEQUAL)		return GL_GEQUAL;
	if (compare == eCompare::COMPARE_EQUAL)		return GL_EQUAL;
	if (compare == eCompare::COMPARE_NOT_EQUAL)	return GL_NOTEQUAL;
	if (compare == eCompare::COMPARE_ALWAYS)		return GL_ALWAYS;

	ERROR_RECOVERABLE("Cannot Find Compare: " + compare);
	return NULL;
}

GLenum Renderer::ToGLType(eRenderDataType dataType)
{
	if (dataType == eRenderDataType::RDT_FLOAT)				return GL_FLOAT;
	if (dataType == eRenderDataType::RDT_UNSIGNED_BYTE)		return GL_UNSIGNED_BYTE;
	if (dataType == eRenderDataType::RDT_TRUE)					return GL_TRUE;
	if (dataType == eRenderDataType::RDT_FALSE)				return GL_FALSE;

	ERROR_RECOVERABLE("Cannot Find Data Type: " + dataType);
	return NULL;
}

GLenum Renderer::TOGLCullMode(eCullMode cullMode)
{
	if (cullMode == eCullMode::CULLMODE_BACK)			return GL_BACK;
	if (cullMode == eCullMode::CULLMODE_FRONT)			return GL_FRONT;
	if (cullMode == eCullMode::CULLMODE_NONE)			return GL_NONE;

	ERROR_RECOVERABLE("Cannot Find CullMode: " + cullMode);
	return NULL;
}

GLenum Renderer::ToGLFillMode(eFillMode fillMode)
{
	if (fillMode == eFillMode::FILLMODE_SOLID)		return GL_FILL;
	if (fillMode == eFillMode::FILLMODE_WIRE)		return GL_LINE;

	ERROR_RECOVERABLE("Cannot Find fillMode: " + fillMode);
	return NULL;
}

GLenum Renderer::ToGLWindOrder(eWindOrder windOrder)
{
	if (windOrder == eWindOrder::WIND_CLOCKWISE)			return GL_CW;
	if (windOrder == eWindOrder::WIND_COUNTER_CLOCKWISE)	return GL_CCW;

	ERROR_RECOVERABLE("Cannot Find windOrder: " + windOrder);
	return NULL;
}

GLenum Renderer::ToGLBlendOp(eBlendOp blendOp)
{
	if (blendOp == eBlendOp::OP_ADD)		return GL_FUNC_ADD;
	if (blendOp == eBlendOp::OP_SUB)		return GL_FUNC_SUBTRACT;
	if (blendOp == eBlendOp::OP_REV_SUB)	return GL_FUNC_REVERSE_SUBTRACT;
	if (blendOp == eBlendOp::OP_MIN)		return GL_MIN;
	if (blendOp == eBlendOp::OP_MAX)		return GL_MAX;

	ERROR_RECOVERABLE("Cannot Find blendOp: " + blendOp);
	return NULL;
}

GLenum Renderer::ToGLBlendFactor(eBlendFactor blendFactor)
{
	if (blendFactor == eBlendFactor::BLEND_ZERO)						return GL_ZERO;					
	if (blendFactor == eBlendFactor::BLEND_ONE)							return GL_ONE;						
	if (blendFactor == eBlendFactor::BLEND_SRC_COLOR)					return GL_SRC_COLOR;				
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_SRC_COLOR)			return GL_ONE_MINUS_SRC_COLOR;		
	if (blendFactor == eBlendFactor::BLEND_DST_COLOR)					return GL_DST_COLOR;				
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_DST_COLOR)			return GL_ONE_MINUS_DST_COLOR;		
	if (blendFactor == eBlendFactor::BLEND_SRC_ALPHA)					return GL_SRC_ALPHA;				
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_SRC_ALPHA)			return GL_ONE_MINUS_SRC_ALPHA;		
	if (blendFactor == eBlendFactor::BLEND_DST_ALPHA)					return GL_DST_ALPHA;				
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_DST_ALPHA)			return GL_ONE_MINUS_DST_ALPHA;		
	if (blendFactor == eBlendFactor::BLEND_CONSTANT_COLOR)				return GL_CONSTANT_COLOR;			
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_CONSTANT_COLOR)	return GL_ONE_MINUS_CONSTANT_COLOR;
	if (blendFactor == eBlendFactor::BLEND_CONSTANT_ALPHA)				return GL_CONSTANT_ALPHA;			
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_CONSTANT_ALPHA)	return GL_ONE_MINUS_CONSTANT_ALPHA;
	if (blendFactor == eBlendFactor::BLEND_SRC_ALPHA_SATURATE)			return GL_SRC_ALPHA_SATURATE;		
	if (blendFactor == eBlendFactor::BLEND_SRC1_COLOR)					return GL_SRC1_COLOR;				
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_SRC1_COLOR)		return GL_ONE_MINUS_SRC1_COLOR;	
	if (blendFactor == eBlendFactor::BLEND_SRC1_ALPHA)					return GL_SRC1_ALPHA;				
	if (blendFactor == eBlendFactor::BLEND_ONE_MINUS_SRC1_ALPHA)		return GL_ONE_MINUS_SRC1_ALPHA;	

	ERROR_RECOVERABLE("Cannot Find blendFactor: " + blendFactor);
	return NULL;
}

GLenum Renderer::ToGLCubeSide(eTextureCubeSide cubeSide)
{
	if (cubeSide == TEXCUBE_RIGHT)		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	if (cubeSide == TEXCUBE_LEFT)		return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	if (cubeSide == TEXCUBE_TOP)		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	if (cubeSide == TEXCUBE_BOTTOM)		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	if (cubeSide == TEXCUBE_BACK)		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	if (cubeSide == TEXCUBE_FRONT)		return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

	ERROR_RECOVERABLE("Cannot Find cubeSide: " + cubeSide);
	return NULL;
}

int Renderer::ToGLPrimitive(eDrawPrimitive drawPrimitive)
{
	if (drawPrimitive == eDrawPrimitive::LINES)			return GL_LINES;
	if (drawPrimitive == eDrawPrimitive::LINE_LOOP)		return GL_LINE_LOOP;
	if (drawPrimitive == eDrawPrimitive::TRIANGLES)		return GL_TRIANGLES;
	if (drawPrimitive == eDrawPrimitive::QUADS)			return GL_QUADS;

	ERROR_RECOVERABLE("Cannot Find drawPrimitive: " + drawPrimitive);
	return NULL;
}

Renderer* Renderer::CreateInstance()
{
	if (g_Renderer == nullptr) 
	{
		g_Renderer = new Renderer();
	}
	return g_Renderer; 
}

Renderer* Renderer::GetInstance()
{
	return g_Renderer;
}

Sampler* Renderer::GetDefaultSampler()
{
	return g_Renderer->m_defaultSampler;
}

#pragma region RenderContext

static HGLRC CreateOldRenderContext( HDC hdc ) 
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(pfd) ); 
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
	if ( pixel_format == NULL ) {
		return NULL; 
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat( hdc, pixel_format, &pfd )) {
		return NULL; 
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext( hdc );
	if (context == NULL) {
		return NULL; 
	}

	// return the context; 
	return context; 
}

//------------------------------------------------------------------------
// Creates a real context as a specific version (major.minor)
static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor ) 
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
											// WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
											// WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
											NULL, NULL,                         // Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;

	BOOL succeeded = wglChoosePixelFormatARB( hdc, 
		format_attribs, 
		nullptr, 
		MAX_PIXEL_FORMATS, 
		formats, 
		(UINT*)&format_count );

	if (!succeeded) {
		return NULL; 
	}

	// Loop through returned formats, till we find one that works
	for (unsigned int i = 0; i < format_count; ++i) {
		pixel_format = formats[i];
		succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context; 
		if (succeeded) {
			break;
		} else {
			DWORD error = GetLastError();
			DebuggerPrintf("Failed to set the format: %u", error);
		}
	}

	if (!succeeded) {
		return NULL; 
	}

	// Okay, HDC is setup to the right format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0; 
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; 
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
	if (context == NULL) {
		return NULL; 
	}

	return context;
}

#pragma endregion
