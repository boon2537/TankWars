#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Transform.hpp"

class Renderable;
class Terrain;
class Material;

class TerrainChunk
{
public:
	~TerrainChunk();
	TerrainChunk();

	void SetUp(Terrain* terrain, const IntVector2& chunkIndex, Material* mat);
	void CleanUp();

public:
	Terrain* m_terrain = nullptr; 
	IntVector2 m_chunkIndex; 
	Transform m_transform;
	Renderable* m_renderable = nullptr; 
};