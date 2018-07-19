#pragma once

#include "Engine/Core/Image.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Physics/Contact.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Math/AABB3.hpp"
#include <vector>
#include <string>

class TerrainChunk;
class Material;
class Renderable;

class Terrain 
{
public:
	~Terrain();
	Terrain();

	void SetUp();
	void FreeAllChunks(); 
	void LoadFromImage(const std::string& path, const AABB2& extents, float min_height, float max_height,const IntVector2& chunk_counts);

	float GetHeight(const Vector2& xz);
	float GetHeightAtDiscreteCoordinate(const IntVector2& coord);
	Vector3 GetPosAtDiscreteCoordinate(const IntVector2& coord);
	Vector3 GetNormalAtDiscreteCoordinate(const IntVector2& coord);
	Vector3 GetTangentAtDiscreteCoordinate(const IntVector2& coord);
	Vector3 GetNormalForXZ(const Vector2& xz);
	AABB2 GetChunkExtents(const IntVector2& chunkIndex);
	AABB2 GetChunkDataExtents(const IntVector2& chunkIndex);

	float GetDistanceFromTerrain(const Vector3& point);
	bool IsPointBelowTerrain(const Vector3& point);
	bool Raycast(RayCastHit3* outResults, const Ray3& ray);

	void DebugCurrentQuad(const Vector2& xz);

	void SetMaterialStamp(const std::string& groundPath, const std::string& waterPath);

public:
	AABB2 m_extents; 
	float m_minHeight; 
	float m_maxHeight; 
	Vector2 m_cellSize; // distance in world space between two cells on the plane
	Image m_image; 
	AABB3 m_bounds;

	Transform m_transform;
	std::vector<TerrainChunk*> m_chunks; 
	IntVector2 m_chunkCounts; 

	std::vector<float> m_heights;
	std::vector<Vector3> m_normals; //cache this!!
	IntVector2 m_dimensions;

	Material* m_terrainChunkMat = nullptr;

	Material* m_waterMat = nullptr;
	Renderable* m_waterRenderable = nullptr;
};
