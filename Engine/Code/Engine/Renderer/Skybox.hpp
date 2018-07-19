#pragma once

#include <string>

class Mesh;
class Shader;
class TextureCube;
class Camera;

class Skybox
{
public:
	~Skybox();
	Skybox(const std::string& path);

	void SetCamera(Camera* cam);
	void Render();

public:
	Camera* m_cameraAttached = nullptr;
	Mesh* m_mesh = nullptr;
	Shader* m_skyboxShader = nullptr;
	TextureCube* m_skyBoxTextureCube = nullptr;
};