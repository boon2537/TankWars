#pragma once

#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Engine/Math/MatrixStack.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Renderer/VertexLayout.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Math/Vector4.hpp"
#include <vector>
#include <map>

#define MAX_LIGHTS 8

#pragma region Built-in Shaders

struct BuiltInShaders
{
	BuiltInShaders(std::string name, char* vertexShader, char* fragmentShader, char* defines = nullptr)
	{
		m_name = name;
		m_vertexShader = vertexShader;
		m_fragmentShader = fragmentShader;
		m_defines = defines;
	}

	std::string m_name;
	char* m_vertexShader;
	char* m_fragmentShader;
	char* m_defines;

	static const BuiltInShaders defaultShader;
	static const BuiltInShaders errorShader;
};

#pragma endregion

struct light_t
{
	Vector4 m_lightColor; // r, g, b, intensity

	Vector3 m_lightPosition;
	float m_uses_shadow;

	Vector3 m_direction = Vector3::one;
	float m_directionFactor = 0.f;

	Vector3 m_attenuation;
	float m_dotInnerAngle = 0.f;

	Vector3 m_spec_attunation;
	float m_dotOuterAngle = 2.f * MathUtils::PI;

	Matrix44 m_shadowVP;

	void SetUp(const Vector3& position, bool usesShadow, const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& spec_attunation, const Vector3& direction, 
		float directionFactor, float dotInnerAngle, float dotOuterAngle, const Matrix44& shadowVP);
	void Disable() { m_lightColor.w = 0.0f; };
};

struct light_factor_t 
{
	Vector3 diffuse; 
	Vector3 specular; 
}; 

struct light_buffer_t
{
	Vector4 m_ambience; //r,g,b,strength
	light_t m_lights[MAX_LIGHTS];
};

struct light_object_buffer_t
{
	float m_specAmount;
	float m_specPower;
	Vector2 m_pad00;
};

struct fog_buffer_t
{
	Vector4 m_fogColor; 
	float m_fogNearPlane;
	float m_fogFarPlane;
	float m_fogNearFactor;
	float m_fogFarFactor;
};

class RenderBuffer;
class Sampler;
class Image;
class Camera;
class Texture;
class FrameBuffer;
class Mesh;
class MeshBuilder;
class ShaderProgram;
class Light;
class Material;
class TextureCube;
struct VertexPCU;

class Renderer
{
public:
	~Renderer();
	Renderer();

	bool RenderStartup(HWND hwnd);
	void PostStartup();
	void GLShutdown();

	void BeginFrame();
	void EndFrame();

	Texture* CreateOrGetTexture(const std::string& path);
	Texture* AddTextureFromImage(const std::string& name, Image* image);
	BitmapFont* CreateOrGetBitmapFont(const char* bitmapFontName);
	Mesh* CreateOrGetMesh(const std::string& path);
	ShaderProgram* CreateOrGetShaderProgram(const std::string& path, const char* defines = nullptr);
	ShaderProgram* RegisterBuiltInShaders(BuiltInShaders builtInShaders);
	void ReloadShaders();
	void SetShaderProgram(ShaderProgram* shaderProgram); //for backward compatibility
	Shader* CreateOrGetShader(const std::string& path);
	Shader* GetShaderByID(const std::string& id);

	Texture* CreateRenderTarget(unsigned int width, unsigned int height, TEXTURE_FORMAT format = TEXTURE_FORMAT::TEXTURE_FORMAT_RGBA8);
	Texture* CreateDepthStencilTarget(unsigned int width, unsigned int height);
	Texture* GetDefaultColorTarget();
	Texture* GetDefaultDepthTarget();
	bool CopyFrameBuffer(FrameBuffer* dst, FrameBuffer* src);
	void EnableDepth(eCompare compare, bool shouldWrite);
	void DisableDepth();
	void ClearDepth(float depth = 1.0f);
	void ClearColor(const Rgba& color);
	void BindTexture(const uint bindPoint = 0, const Texture* texture = nullptr);
	void BindCubeMap(const uint bindPoint, const TextureCube* textureCube);
	void BindSampler(const uint bindPoint = 0, Sampler* sampler = nullptr);
	void BindMeshToProgram(ShaderProgram *program, Mesh *mesh); 
	void SetWireFrameDrawModeActive(bool active);
	void BindShaderProgram(ShaderProgram* shaderProgram); 
	void BindRenderState(const RenderState_t& state);
	void SetShader(Shader* shader); 
	void BindMaterial(Material* mat);

	void DrawAABB(const AABB2& bounds, const Rgba& color);
	void DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint);
	void DrawTexturedAABB2_3D(const Vector3& position, const Vector2& dimensions, const Vector2& pivot, const Vector2& scale, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Matrix44& orientation, const Rgba& tint);
	void DrawTextureFromVertexArray(const VertexPCU* verts, int numVerts, const Texture& texture);
	void DrawSprite(const Vector3& position, Sprite* sprite, const Matrix44& orientation = Matrix44::identity, const Vector2& scale = Vector2(1, 1), const Rgba& tint = Rgba::white);
	void DrawPoly(int numSides, float orientation, float radius, const Vector2& center, const Rgba& color);
	void DrawFromVertexArray(const Vector2 vertexArray[], int arraySize, const Vector2& center, float orientation, float scale, const Rgba& color);
	void DrawDashedCircle(const Vector2& center, float radius, const Rgba& color);
	void DrawLine(const Vector2& p1, const Vector2& p2, const Rgba& color);
	void DrawLine(const Vector2& p1, const Vector2& p2, const Rgba& colorBegin, const Rgba& colorEnd);
	void DrawLine(const Vector3& p1, const Vector3& p2, const Rgba& color);
	void DrawLine(const Vector3& p1, const Vector3& p2, const Rgba& colorBegin, const Rgba& colorEnd);

	void DrawText2D(const Vector2& drawMins,
		const std::string& asciiText,
		float cellHeight,
		const Rgba& tint = Rgba::white,
		float aspectScale = 1.f, // multiplied by the font’s inherent m_baseAspect
		const BitmapFont* font = nullptr);
	void DrawTextInBox2D(const AABB2& bounds, 
		const std::string& asciiText,
		float cellHeight, 
		const Rgba & tint = Rgba::white, 
		float aspectScale = 1.f,
		const BitmapFont * font = nullptr,
		eTextDrawMode textDrawMode = eTextDrawMode::OVERRUN,
		const Vector2& alignment = Vector2(0, 0));
	void DrawTextInBox3D(const Vector3& position, 
		const AABB2 & bounds,
		const Matrix44& orientation,
		const std::string& asciiText,
		float cellHeight, 
		const Vector2& alignment = Vector2(0, 0),
		const Rgba & tint = Rgba::white, 
		eTextDrawMode textDrawMode = eTextDrawMode::OVERRUN,
		float aspectScale = 1.f,
		const BitmapFont * font = nullptr);
	void DrawText3D(const Vector3& botLeftText, std::string& asciiText, float cellHeight, const Matrix44& orientation, const Rgba& tint = Rgba::white, const BitmapFont* font = nullptr, float aspectScale = 1.f);
	
	void DrawCube(const Vector3& center, 
		const Vector3& dimensions, // width, height, depth
		const Rgba& tint = Rgba::white,
		const AABB2& uv_top = AABB2::ZERO_TO_ONE, 
		const AABB2& uv_side = AABB2::ZERO_TO_ONE, 
		const AABB2& uv_bottom = AABB2::ZERO_TO_ONE);  
	void DrawTexturedCube(const Vector3& center, 
		const Vector3& dimensions, // width, height, depth
		Texture* texture,
		const Rgba& tint = Rgba::white,
		const AABB2& uv_top = AABB2::ZERO_TO_ONE, 
		const AABB2& uv_side = AABB2::ZERO_TO_ONE, 
		const AABB2& uv_bottom = AABB2::ZERO_TO_ONE);  
	void DrawUVSphere(const Vector3& center, float radius, const Rgba& tint, Texture* texture = nullptr);
	void DrawPlane(const Vector3& center, const Vector3& right, const Vector3& up, const AABB2& bounds, Texture* texture = nullptr, const Rgba& tint = Rgba::white);

	void DrawMeshImmediate(const VertexPCU* verts, int numVerts, eDrawPrimitive drawPrimitive, uint icount = 0, uint* indices = nullptr);
	void DrawMesh(Mesh* mesh, const Matrix44& model = Matrix44());
	void DrawMeshWithMaterial(Material* mat, Mesh* mesh, const Matrix44& model = Matrix44());

	void SetCamera(Camera* camera);
	Camera* GetActiveCamera();
	void SetProjectionOrtho(float width, float height, float orthoNear, float orthoFar);
	void SetProjectionMatrix(const Matrix44& projection);

	void PushMatrix();
	void Translate2DInMatrix(const Vector2& center);
	void Rotate2DInMatrix(const float orientation);
	void UniformScale2DInMatrix(const float scale);
	void PopMatrix();

	void SetAdditiveBlendingActive(bool active);

	void SetUniform(const char* name, float f);
	void SetUniform(const char* name, const Vector3& v);
	void SetUniform(const char* name, const Vector4& v);
	void SetUniform(const char* name, const Rgba& color);
	void ApplyEffect(Shader* shader);
	void FinishEffects();

	// Lighting Methods
	void SetAmbientLight(float intensity, const Rgba& color); 
	void DisableAllLights(); 
	void EnableLight(Light* light, uint index);
	void SetSpecularConstants(float specAmount, float specPower); 
	void SetFog(const Rgba& color, float nearPlane, float farPlane, float nearFactor, float farFactor);

	void UpdateTimeBlock(float currentTime);

	Texture* GetPlainWhiteTexture();
	void TakeScreenShot();

public:
	static void TextureGetInternalFormat(GLuint* out_internalFormat, GLuint* out_channels, GLuint* out_pixelLayout, GLuint format);

	static GLenum ToGLCompare(eCompare compare);
	static GLenum ToGLType(eRenderDataType dataType); 
	static GLenum TOGLCullMode(eCullMode cullMode);
	static GLenum ToGLFillMode(eFillMode fillMode);
	static GLenum ToGLWindOrder(eWindOrder windOrder);
	static GLenum ToGLBlendOp(eBlendOp blendOp);
	static GLenum ToGLBlendFactor(eBlendFactor blendFactor);
	static GLenum ToGLCubeSide(eTextureCubeSide cubeSide);
	static int ToGLPrimitive(eDrawPrimitive drawPrimitive);

	static Renderer* CreateInstance();
	static Renderer* GetInstance(); 
	static Sampler* GetDefaultSampler();

public:
	static HMODULE gGLLibrary;
	static HWND gGLwnd;    // window our context is attached to; 
	static HDC gHDC;    // our device context
	static HGLRC gGLContext;    // our rendering context; 

private:
	std::map<std::string, Texture*> m_textureMap;
	std::map<std::string, BitmapFont*> m_loadedFonts;
	std::map<std::string, Mesh*> m_loadedMesh;
	std::map<std::string, ShaderProgram*> m_loadedShaderPrograms;
	std::map<std::string, ShaderProgram*> m_builtInShaderPrograms;
	std::map<std::string, Shader*> m_loadedShaders;
	Mesh* m_immediateMesh;

	MatrixStack m_matrixStack;

	GLuint m_default_vao;
	RenderBuffer* m_immediateBuffer = nullptr;
	RenderBuffer* m_tempRenderBuffer = nullptr;
	Shader* m_defaultShader = nullptr; //All old state switching functions should affect the default shader.  
	Shader* m_currentShader = nullptr;
	Sampler* m_defaultSampler = nullptr;
	Sampler* m_shadowSampler = nullptr;
	Texture* m_defaultColorTarget = nullptr;
	Texture* m_defaultDepthTarget = nullptr;
	Texture* m_plainWhiteTexture = nullptr; //for drawing non-textured stuff

	Material* m_defaultMaterial = nullptr;

	Camera* m_activeCamera = nullptr;
	Camera* m_defaultCamera = nullptr;

	Texture* m_effectTarget;
	Texture* m_effectScratch;
	Camera* m_effectCamera;

	UniformBuffer m_lightBuffer;
	UniformBuffer m_lightObjectBuffer;
	UniformBuffer m_fogBuffer;

	//hacky
	float m_gameTimeRef = 0;
};

static HGLRC CreateOldRenderContext(HDC hdc);
static HGLRC CreateRealRenderContext(HDC hdc, int major, int minor);
