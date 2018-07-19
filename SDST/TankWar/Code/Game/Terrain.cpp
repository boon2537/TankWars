#include "Game/Terrain.hpp"
#include "Game/TerrainChunk.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Math/Vector4.hpp"

Terrain::~Terrain()
{
	FreeAllChunks();
}

Terrain::Terrain()
{
}

void Terrain::SetUp()
{
	m_cellSize.x = m_extents.GetDimensions().x / m_dimensions.x;
	m_cellSize.y = m_extents.GetDimensions().y / m_dimensions.y;

	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			//use point sample for now
			int image_x = (int) RangeMapFloat((float) x, 0, (float) m_dimensions.x, 0, (float) m_image.GetDimensions().x);
			int image_y = (int) RangeMapFloat((float) y, 0, (float) m_dimensions.y, 0, (float) m_image.GetDimensions().y);

			float height = RangeMapFloat(m_image.GetTexel(image_x, image_y).r, 0, 255, m_minHeight, m_maxHeight);
			m_heights.push_back(height);
		}
	}

	//Cache normals
	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
		{
			IntVector2 coord = IntVector2(x, y);

			Vector3 du = GetPosAtDiscreteCoordinate(coord + IntVector2(1, 0)) - GetPosAtDiscreteCoordinate(coord - IntVector2(1, 0));
			Vector3 tangent =  du.GetNormalized();

			Vector3 dv = GetPosAtDiscreteCoordinate(coord + IntVector2(0, 1)) - GetPosAtDiscreteCoordinate(coord - IntVector2(0, 1));
			Vector3 bitan = dv.GetNormalized();

			Vector3 normal = CrossProduct(bitan, tangent);
			m_normals.push_back(normal);
		}
	}

	//Setup chunks
	for (int y = 0; y < m_chunkCounts.y; y++)
	{
		for (int x = 0; x < m_chunkCounts.x; x++)
		{
			TerrainChunk* chunk = new TerrainChunk();
			chunk->SetUp(this, IntVector2(x, y), m_terrainChunkMat);
			m_chunks.push_back(chunk);
		}
	}

	//Setup water
	float waterHeight = 10.f;

	MeshBuilder mb = MeshBuilder();
	mb.Begin(eDrawPrimitive::TRIANGLES, true);
	mb.SetColor(Rgba::white);
	IntVector2 bl_coord = IntVector2(0, 0);
	Vector3 bl = Vector3(GetPosAtDiscreteCoordinate(bl_coord).x, waterHeight, GetPosAtDiscreteCoordinate(bl_coord).z);
	mb.SetUV(Vector2::zero);
	mb.SetNormal(GetNormalAtDiscreteCoordinate(bl_coord));
	mb.SetTangent(Vector4(GetTangentAtDiscreteCoordinate(bl_coord), 1));
	int index = mb.PushVertex(bl);

	IntVector2 br_idx = IntVector2(m_dimensions.x - 1, 0);
	Vector3 br = Vector3(GetPosAtDiscreteCoordinate(br_idx).x, waterHeight, GetPosAtDiscreteCoordinate(br_idx).z);
	mb.SetUV(Vector2(128, 0));
	mb.SetNormal(GetNormalAtDiscreteCoordinate(br_idx));
	mb.SetTangent(Vector4(GetTangentAtDiscreteCoordinate(br_idx), 1));
	mb.PushVertex(br);

	IntVector2 tl_idx = IntVector2(0, m_dimensions.y - 1);
	Vector3 tl = Vector3(GetPosAtDiscreteCoordinate(tl_idx).x, waterHeight, GetPosAtDiscreteCoordinate(tl_idx).z);
	mb.SetUV(Vector2(0, 128));
	mb.SetNormal(GetNormalAtDiscreteCoordinate(tl_idx));
	mb.SetTangent(Vector4(GetTangentAtDiscreteCoordinate(tl_idx), 1));
	mb.PushVertex(tl);

	IntVector2 tr_idx = IntVector2(m_dimensions.x - 1, m_dimensions.y - 1);
	Vector3 tr = Vector3(GetPosAtDiscreteCoordinate(tr_idx).x, waterHeight, GetPosAtDiscreteCoordinate(tr_idx).z);
	mb.SetUV(Vector2(128, 128));
	mb.SetNormal(GetNormalAtDiscreteCoordinate(tr_idx));
	mb.SetTangent(Vector4(GetTangentAtDiscreteCoordinate(tr_idx), 1));
	mb.PushVertex(tr);

	mb.AddQuad(index + 0, index + 1, index + 2, index + 3);
	mb.End();

	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);
	m_waterRenderable = new Renderable(mesh, &m_transform, m_waterMat);
	RenderScene::GetCurrentScene()->AddRenderable(m_waterRenderable);

	//Set bounds
	// for compensate camera's delta so we don't raycast when camera is out of bound and hit bound from the outside
	Vector3 offset = Vector3(60);
	m_bounds = AABB3(Vector3(m_extents.mins.x, m_minHeight, m_extents.mins.y) - offset, Vector3(m_extents.maxs.x, m_maxHeight, m_extents.maxs.y) + offset);
}

void Terrain::FreeAllChunks()
{
	for each (TerrainChunk* chunk in m_chunks)
	{
		delete chunk;
		chunk = nullptr;
	}
	m_chunks.clear();
}

void Terrain::LoadFromImage(const std::string& path, const AABB2& extents, float min_height, float max_height, const IntVector2& chunk_counts)
{
	m_image = Image(path);
	m_extents = extents;
	m_minHeight = min_height;
	m_maxHeight = max_height;
	m_chunkCounts = chunk_counts;
	m_dimensions = m_image.GetDimensions();
}

float Terrain::GetHeight(const Vector2& xz)
{
	float x_data = RangeMapFloat(xz.x, m_extents.mins.x, m_extents.maxs.x, 0, (float) m_dimensions.x - 1);
	float z_data = RangeMapFloat(xz.y, m_extents.mins.y, m_extents.maxs.y, 0, (float) m_dimensions.y - 1);

	IntVector2 bl = IntVector2((int) x_data, (int) z_data);
	float height_bl = GetHeightAtDiscreteCoordinate(bl);
	float height_br = GetHeightAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y));
	float height_tl = GetHeightAtDiscreteCoordinate(IntVector2(bl.x, bl.y + 1));
	float height_tr = GetHeightAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y + 1));

	float height_bottom = Interpolate(height_bl, height_br, fmodf(x_data, 1));
	float height_top = Interpolate(height_tl, height_tr, fmodf(x_data, 1));
	float height = Interpolate(height_bottom, height_top, fmodf(z_data, 1));

	return height;
}

float Terrain::GetHeightAtDiscreteCoordinate(const IntVector2& coord)
{
	uint index = (coord.y * m_dimensions.x) + coord.x;

	//temporary fix
	return m_heights[ClampInt(index, 0, (int) m_heights.size() - 1)];
}

Vector3 Terrain::GetPosAtDiscreteCoordinate(const IntVector2& coord)
{
	Vector2 origin_xz = m_extents.mins;
	Vector2 xz = origin_xz + Vector2(coord.x * m_cellSize.x, coord.y * m_cellSize.y);

	float height = GetHeightAtDiscreteCoordinate(coord);

	return Vector3(xz.x, height, xz.y);
}

Vector3 Terrain::GetNormalAtDiscreteCoordinate(const IntVector2& coord)
{
	uint index = (coord.y * m_dimensions.x) + coord.x;

	return m_normals[ClampInt(index, 0, (int) m_heights.size() - 1)];
}

Vector3 Terrain::GetTangentAtDiscreteCoordinate(const IntVector2& coord)
{
	Vector3 du = GetPosAtDiscreteCoordinate(coord + IntVector2(1, 0)) - GetPosAtDiscreteCoordinate(coord - IntVector2(1, 0));
	Vector3 tangent = du.GetNormalized();

	return tangent;
}

Vector3 Terrain::GetNormalForXZ(const Vector2& xz)
{
	float x_data = RangeMapFloat(xz.x, m_extents.mins.x, m_extents.maxs.x, 0, (float) m_dimensions.x - 1);
	float z_data = RangeMapFloat(xz.y, m_extents.mins.y, m_extents.maxs.y, 0, (float) m_dimensions.y - 1);

	IntVector2 bl = IntVector2((int) x_data, (int) z_data);
	Vector3 normal_bl = GetNormalAtDiscreteCoordinate(bl);
	Vector3 normal_br = GetNormalAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y));
	Vector3 normal_tl = GetNormalAtDiscreteCoordinate(IntVector2(bl.x, bl.y + 1));
	Vector3 normal_tr = GetNormalAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y + 1));

	Vector3 normal_bottom = Interpolate(normal_bl, normal_br, fmodf(x_data, 1));
	Vector3 normal_top = Interpolate(normal_tl, normal_tr, fmodf(x_data, 1));
	Vector3 normal = Interpolate(normal_bottom, normal_top, fmodf(z_data, 1));

	return normal;
}

AABB2 Terrain::GetChunkExtents(const IntVector2& chunkIndex)
{
	float height = m_extents.GetDimensions().x / m_chunkCounts.x;
	float width = m_extents.GetDimensions().y / m_chunkCounts.y;

	return AABB2(width * chunkIndex.x, height * chunkIndex.y, width * (chunkIndex.x + 1), height * (chunkIndex.y + 1));
}

AABB2 Terrain::GetChunkDataExtents(const IntVector2& chunkIndex)
{
	int height = m_dimensions.x / m_chunkCounts.x;
	int width = m_dimensions.y / m_chunkCounts.y;

	return AABB2((float) (width * chunkIndex.x), (float) (height * chunkIndex.y), (float) (width * (chunkIndex.x + 1)), (float) (height * (chunkIndex.y + 1)));
}

float Terrain::GetDistanceFromTerrain(const Vector3& point)
{
	//just get y difference for now
	float height = GetHeight(Vector2(point.x, point.z));
	return point.y - height;
}

bool Terrain::IsPointBelowTerrain(const Vector3& point)
{
	return GetHeight(Vector2(point.x, point.z)) > point.y;
}

bool Terrain::Raycast(RayCastHit3* outResults, const Ray3& ray)
{
	*outResults = RayCheckAABB3(ray, m_bounds);

	//If we don't hit the bounds, early out without a hit
	if (!outResults->hit)
	{
		//DebugLogf("not in bounds", Rgba::white, 0);
		*outResults = RayCastHit3();
		return false;
	}

	//If our first point is below the terrain, we throw out the ray
	if (IsPointBelowTerrain(ray.start))
	{
		//DebugLogf("below to begin with", Rgba::white, 0);
		*outResults = RayCastHit3();
		return false;
	}

	float stepSize = 0.5f * m_cellSize.x;
	float rayDistance = GetDistance(ray.start, outResults->position);
	uint stepsTilEnd = (uint) std::ceil(rayDistance / stepSize);
	uint stepCount = 0;

	Vector3 endOfRay = outResults->position;
	Vector3 abovePoint = ray.start;
	Vector3 belowPoint = ray.Evaluate(stepCount * stepSize);

	//Step along the ray until we detect we're below the terrain
	while (stepCount < stepsTilEnd && !IsPointBelowTerrain(belowPoint))
	{
		stepCount++;
		belowPoint = ray.Evaluate(stepCount * stepSize);
	}

	//We we reach the end of our ray and still don't have a point below the terrain, we didn't hit it, so early out
	if (stepCount == stepsTilEnd && !IsPointBelowTerrain(belowPoint))
	{
		//DebugLogf("reached end", Rgba::white, 0);
		*outResults = RayCastHit3();
		return false;
	}

	//If the point we are aiming is outside of terrain, early out 
	//this happen because of bounds offset in SetUp
	if (!m_extents.IsPointInside(Vector2(belowPoint.x, belowPoint.z)))
	{
		//DebugLogf("out of bound", Rgba::white, 0);
		*outResults = RayCastHit3();
		return false;
	}

	Vector3 point = Average(abovePoint, belowPoint); 

	stepCount = 0; 
	uint MAX_STEPS = 1000;
	while (!IsMostlyEqual(abs(GetDistanceFromTerrain(point)), 0.f) && (stepCount < MAX_STEPS)) 
	{
		if (IsPointBelowTerrain(point)) { 
			belowPoint = point; 
		} else { 
			abovePoint = point; 
		}

		point = Average(abovePoint, belowPoint); 

		stepCount++;  
	}

	*outResults = RayCastHit3(point, GetNormalForXZ(Vector2(point)));
	return true;
}

void Terrain::DebugCurrentQuad(const Vector2& xz)
{
	float x_data = RangeMapFloat(xz.x, m_extents.mins.x, m_extents.maxs.x, 0, (float) m_dimensions.x - 1);
	float z_data = RangeMapFloat(xz.y, m_extents.mins.y, m_extents.maxs.y, 0, (float) m_dimensions.y - 1);

	IntVector2 bl = IntVector2((int) x_data, (int) z_data);

	//height
// 	DebugRenderPoint(0, GetPosAtDiscreteCoordinate(bl), Rgba::red, Rgba::red);
// 	DebugRenderPoint(0, GetPosAtDiscreteCoordinate(IntVector2(bl.x, bl.y + 1)), Rgba::blue, Rgba::red);
// 	DebugRenderPoint(0, GetPosAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y)), Rgba::blue, Rgba::blue);
// 	DebugRenderPoint(0, GetPosAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y + 1)), Rgba::blue, Rgba::blue);

	//normal
	DebugRenderLineSegment(0, GetPosAtDiscreteCoordinate(bl), Rgba::red, GetPosAtDiscreteCoordinate(bl) + (GetNormalAtDiscreteCoordinate(bl) * 4), Rgba::red);
	DebugRenderLineSegment(0, GetPosAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y)), Rgba::red, GetPosAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y)) + (GetNormalAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y)) * 4), Rgba::red);
	DebugRenderLineSegment(0, GetPosAtDiscreteCoordinate(IntVector2(bl.x, bl.y + 1)), Rgba::red, GetPosAtDiscreteCoordinate(IntVector2(bl.x, bl.y + 1)) + (GetNormalAtDiscreteCoordinate(IntVector2(bl.x, bl.y + 1)) * 4), Rgba::red);
	DebugRenderLineSegment(0, GetPosAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y + 1)), Rgba::red, GetPosAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y + 1)) + (GetNormalAtDiscreteCoordinate(IntVector2(bl.x + 1, bl.y + 1)) * 4), Rgba::red);
}

void Terrain::SetMaterialStamp(const std::string& groundPath, const std::string& waterPath)
{
	m_terrainChunkMat = Material::GetOrCreate(groundPath);
	m_waterMat = Material::GetOrCreate(waterPath);
}
