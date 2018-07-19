#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Skybox.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include <algorithm>

Transform* ForwardRenderingPath::s_lightFocalPoint = nullptr;

ForwardRenderingPath::~ForwardRenderingPath()
{
	delete m_shadowCamera;
}

ForwardRenderingPath::ForwardRenderingPath(Renderer* r)
	: m_renderer(r)
{
	m_shadowCamera = new Camera();
	m_shadowCamera->SetColorTarget(r->GetDefaultColorTarget());
}

void ForwardRenderingPath::Render(RenderScene* scene)
{
	// pre-step - generate all shadow maps
	for each (Light* light in scene->m_lights) 
	{
		if (light->IsDirectionalLight()) 
		{
			RenderShadowCastingObjectsForLight(light, scene); 
		}
	}

	// "may" (really should) want to sort cameras
	for each (Camera* camera in scene->m_cameras) 
	{
		RenderSceneForCamera(camera, scene); 
	}
}

void ForwardRenderingPath::RenderSceneForCamera(Camera* cam, RenderScene* scene)
{
	m_renderer->SetCamera(cam);

	if (cam->ShouldClear()) 
	{
		m_renderer->ClearColor(Rgba::black); 
		m_renderer->ClearDepth(1.0f); 
	}

	std::vector<DrawCall> drawCalls; 

	for each (Renderable* renderable in scene->m_renderables) 
	{
		Light* lights[MAX_LIGHTS];
		uint lightCount; 

		if (renderable->UseLight()) 
		{
			scene->GetMostContributingLights(&lightCount, &*lights, renderable->GetTransform()->GetWorldPosition()); 
		}

		DrawCall dc; 
		dc.m_model = renderable->GetModelMatrix();
		dc.m_mesh = renderable->GetMesh();
		dc.m_material = renderable->GetMaterial();
		dc.m_layer = dc.m_material->GetShader()->m_sort;
		dc.m_queue = dc.m_material->GetShader()->m_renderQueue;
		dc.m_lightCount = lightCount;
		for (uint i = 0; i < MAX_LIGHTS; i++)
			dc.m_lights[i] = lights[i];

		drawCalls.push_back(dc); 
	}

	SortDrawsBySortOrder(drawCalls);

	//draw skybox before everything
	if (m_skybox != nullptr)
		m_skybox->Render();

	for each (DrawCall dc in drawCalls) 
	{
		if (dc.m_lightCount > 0)
		{
			// enable all active lights
			m_renderer->DisableAllLights();
			for (uint i = 0; i < dc.m_lightCount; i++) 
			{
				m_renderer->EnableLight(dc.m_lights[i], i);
			}
		}

		m_renderer->DrawMeshWithMaterial(dc.m_material, dc.m_mesh, dc.m_model);
	}
}

void ForwardRenderingPath::RenderShadowCastingObjectsForLight(Light* light, RenderScene* scene)
{
	Renderer* r = Renderer::GetInstance();

	if (m_shadowShader == nullptr)
		m_shadowShader = new Shader("Data/Shaders/shadow.xml");

	TODO("Do spot light later");
	if (light->IsDirectionalLight())
	{
		m_shadowCamera->SetProjectionOrtho(light->m_shadow_ppu, light->m_shadow_ppu, 0.f, light->m_shadowDistance);
	}

	if (s_lightFocalPoint != nullptr)
	{
		Matrix44 mat = light->m_transform.GetWorldMatrix();
		Vector3 focalPos = s_lightFocalPoint->GetWorldPosition();
		mat.Tx += focalPos.x;
		mat.Tz += focalPos.z;

		m_shadowCamera->SetLocalMatrix(mat);
		DebugRenderBasis(0, m_shadowCamera->m_transform.GetWorldMatrix());
		//DebugLogf(m_shadowCamera->m_transform.GetWorldPosition().ToString(), Rgba::white, 0);
	}
	else
	{
		m_shadowCamera->SetLocalMatrix(light->m_transform.GetWorldMatrix());
	}

	m_shadowCamera->SetDepthStencilTarget(light->GetorCreateShadowTexture()); 

	r->SetCamera(m_shadowCamera);
	r->SetShader(m_shadowShader);
	m_renderer->ClearColor(Rgba::black); 
	m_renderer->ClearDepth(1.0f); 

	// set_camera should call glViewport with resolution of render target
	light->m_shadowVP = m_shadowCamera->GetViewProjection();

	//renderer->set_material( "depth_only" );????
	for each (Renderable* renderable in scene->m_renderables)
	{
		r->DrawMeshWithMaterial(renderable->GetMaterial(), renderable->GetMesh(), renderable->GetModelMatrix());
	}
}

void ForwardRenderingPath::SortDrawsBySortOrder(std::vector<DrawCall>& drawCalls)
{
	std::sort(drawCalls.begin(), drawCalls.end(), [](const DrawCall& a, const DrawCall& b) -> bool
	{
		if (a.m_layer == b.m_layer)
		{
			return a.m_queue < b.m_queue;
		}

		return a.m_layer < b.m_layer;
	});
}
