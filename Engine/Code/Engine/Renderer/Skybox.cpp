#include "Engine/Renderer/Skybox.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/Camera.hpp"

Skybox::~Skybox()
{
}

Skybox::Skybox(const std::string& path)
{
	MeshBuilder mb;
	mb.AddCube(Vector3::zero, Vector3(1, 1, 1), Vector3::one, Rgba::white);
	m_mesh = new Mesh();
	m_mesh->FromBuilderForType<VertexPCU>(mb);

	m_skyBoxTextureCube = new TextureCube();
	m_skyBoxTextureCube->MakeFromImages(path.c_str());

	Renderer* r = Renderer::GetInstance();
	m_skyboxShader = r->CreateOrGetShader("Data/Shaders/skybox.xml");
}

void Skybox::SetCamera(Camera* cam)
{
	m_cameraAttached = cam;
}

void Skybox::Render()
{
	Renderer* r = Renderer::GetInstance();

	r->SetShader(m_skyboxShader);
	r->BindSampler(8);
	r->BindCubeMap(8, m_skyBoxTextureCube);
	r->DrawMesh(m_mesh, m_cameraAttached->m_transform.GetWorldMatrix());
}
