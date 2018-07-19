#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

class Renderable;
class Light;
class Camera;

class RenderScene 
{
public:
	~RenderScene();
	RenderScene();

	void AddRenderable(Renderable *r);
	void AddLight(Light* l); 
	void AddCamera(Camera* c);

	void RemoveRenderable(Renderable *r);
	void RemoveLight(Light* l); 
	void RemoveCamera(Camera* c);

	void GetMostContributingLights(uint* lightCount, Light** lights, const Vector3& position); 

public:
	static RenderScene* SetCurrentScene(RenderScene* currentScene);
	static RenderScene* GetCurrentScene();

public:
	std::vector<Renderable*> m_renderables;
	std::vector<Light*> m_lights; 
	std::vector<Camera*> m_cameras; 
};
