#pragma once

#include "Engine/Math/Matrix44.hpp"

class Material;
class Transform;
class Mesh;

class Renderable
{
public:
	~Renderable();
	Renderable();
	Renderable(Mesh* mesh, Transform* transform, Material* material);

	void Render();
	bool UseLight();

	void SetMesh(Mesh* mesh); 
	Mesh* GetMesh() const;

	void SetMaterial(Material* mat); 
	Material* GetSharedMaterial();
	Material* GetMaterial();

	void SetTransform(Transform* transform);
	Transform* GetTransform() const;
	Matrix44 GetModelMatrix() const; 

public:
	Mesh* m_mesh = nullptr;
	Transform* m_transform = nullptr;

	Material* m_material = nullptr;
	Material* m_sharedMaterial = nullptr;

	//hack to see if wanna get lit
	bool m_isLit = true;
};
