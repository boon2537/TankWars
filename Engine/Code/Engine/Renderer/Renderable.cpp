#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"

Renderable::~Renderable()
{
	delete m_material;
	m_material = nullptr;
	m_sharedMaterial = nullptr;

	if (m_mesh->m_isResource)
	{
		delete m_mesh;
		m_mesh = nullptr;
	}
}

Renderable::Renderable()
{
}

Renderable::Renderable(Mesh* mesh, Transform* transform, Material* material)
{
	SetMesh(mesh);
	SetTransform(transform);
	SetMaterial(material);
}

void Renderable::Render()
{
	if (m_transform != nullptr)
	{
		Renderer* r = Renderer::GetInstance();
		r->DrawMeshWithMaterial(m_material, m_mesh, GetModelMatrix());
	}
}

bool Renderable::UseLight()
{
	TODO("find a better way to see if shader wants to use light");
	return m_isLit;
}

void Renderable::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
}

Mesh* Renderable::GetMesh() const
{
	return m_mesh;
}

void Renderable::SetMaterial(Material* mat)
{
	if (mat->m_is_resource)
	{
		m_sharedMaterial = mat;
		m_material = nullptr;
	}
	else
	{
		m_material = mat;
	}
}

Material* Renderable::GetSharedMaterial()
{
	return m_sharedMaterial;
}

Material* Renderable::GetMaterial()
{
	if (m_material == nullptr) 
	{
		m_material = m_sharedMaterial->Clone(); 
		m_material->m_is_resource = false;
	}

	return m_material; 
}

void Renderable::SetTransform(Transform* transform)
{
	m_transform = transform;
}

Transform* Renderable::GetTransform() const
{
	return m_transform;
}

Matrix44 Renderable::GetModelMatrix() const
{
	return m_transform->GetWorldMatrix();
}
