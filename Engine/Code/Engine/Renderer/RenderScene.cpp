#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Core/STL_Utils.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <algorithm>

static RenderScene* g_RenderScene = nullptr; 

RenderScene::~RenderScene()
{
}

RenderScene::RenderScene()
{
}

void RenderScene::AddRenderable(Renderable* r)
{
	m_renderables.push_back(r);
}

void RenderScene::AddLight(Light* l)
{
	m_lights.push_back(l);
}

void RenderScene::AddCamera(Camera* c)
{
	m_cameras.push_back(c);
}

void RenderScene::RemoveRenderable(Renderable* r)
{
	QuickErase(m_renderables, r);
}

void RenderScene::RemoveLight(Light* l)
{
	QuickErase(m_lights, l);
}

void RenderScene::RemoveCamera(Camera* c)
{
	QuickErase(m_cameras, c);
}

void RenderScene::GetMostContributingLights(uint* lightCount, Light** lights, const Vector3& position)
{
	std::sort(m_lights.begin(), m_lights.end(), [position](Light* a, Light* b) -> bool
	{
		return a->GetAffectorRate(position) > b->GetAffectorRate(position);
	});

	int maxSize = MinInt((int) m_lights.size(), MAX_LIGHTS);
	uint returnCount = 0;
	for (int i = 0; i < maxSize; i++)
	{
		*lights = m_lights[i];
		lights++;
		returnCount++;
	}

	*lightCount = returnCount;
}

RenderScene* RenderScene::SetCurrentScene(RenderScene* currentScene)
{
	return g_RenderScene = currentScene;
}

RenderScene* RenderScene::GetCurrentScene()
{
	return g_RenderScene;
}
