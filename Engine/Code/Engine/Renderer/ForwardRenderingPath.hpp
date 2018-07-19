#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Transform.hpp"

class RenderScene;
class Camera;
class Renderer;
class Mesh;
class Light;
class Skybox;
class Shader;

class DrawCall 
{
public:
	Matrix44 m_model; 
	Mesh* m_mesh; 
	Material* m_material;
	// Camera *m_camera; 
	// int pass_number

	uint m_lightCount; 
	Light* m_lights[MAX_LIGHTS]; 

	int m_layer; //sort order
	uint m_queue; //Alpha/Opaque
};

class ForwardRenderingPath
{
public:
	~ForwardRenderingPath();
	ForwardRenderingPath(Renderer* r);

	void Render(RenderScene* scene);
	void RenderSceneForCamera(Camera* cam, RenderScene* scene);
	void RenderShadowCastingObjectsForLight(Light* light, RenderScene*scene);

	void SortDrawsBySortOrder(std::vector<DrawCall>& drawCalls);

public:
	static Transform* s_lightFocalPoint;

	Renderer* m_renderer = nullptr;
	Skybox* m_skybox = nullptr;

	Camera* m_shadowCamera = nullptr;
	Shader* m_shadowShader = nullptr;
};
