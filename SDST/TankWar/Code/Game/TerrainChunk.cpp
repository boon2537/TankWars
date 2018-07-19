#include "Game/TerrainChunk.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/Terrain.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderable.hpp"

TerrainChunk::~TerrainChunk()
{
	CleanUp();
}

TerrainChunk::TerrainChunk()
{
}

void TerrainChunk::SetUp(Terrain* terrain, const IntVector2& chunkIndex, Material* mat)
{
	m_terrain = terrain;
	m_chunkIndex = chunkIndex;

	MeshBuilder mb = MeshBuilder();

	mb.Begin(eDrawPrimitive::TRIANGLES, true);
	mb.SetColor(Rgba::white);
	int index = 0;

	AABB2 dataPoint = terrain->GetChunkDataExtents(m_chunkIndex);

	for (int y = (int) dataPoint.mins.y; y < dataPoint.maxs.y; y++)
	{
		for (int x = (int) dataPoint.mins.x; x < dataPoint.maxs.x; x++)
		{
			IntVector2 bl_idx = IntVector2(x, y);
			Vector3 bl = terrain->GetPosAtDiscreteCoordinate(bl_idx);
			mb.SetUV(Vector2((float) x / terrain->m_dimensions.x, (float) y / terrain->m_dimensions.y));
			mb.SetNormal(terrain->GetNormalAtDiscreteCoordinate(bl_idx));
			mb.SetTangent(Vector4(terrain->GetTangentAtDiscreteCoordinate(bl_idx), 1));
			index = mb.PushVertex(bl);

			IntVector2 br_idx = IntVector2(MinInt(x + 1, terrain->m_dimensions.x - 1), y);
			Vector3 br = terrain->GetPosAtDiscreteCoordinate(br_idx);
			mb.SetUV(Vector2((float) (x + 1) / terrain->m_dimensions.x, (float) y / terrain->m_dimensions.y));
			mb.SetNormal(terrain->GetNormalAtDiscreteCoordinate(br_idx));
			mb.SetTangent(Vector4(terrain->GetTangentAtDiscreteCoordinate(br_idx), 1));
			mb.PushVertex(br);

			IntVector2 tl_idx = IntVector2(x, MinInt(y + 1, terrain->m_dimensions.y - 1));
			Vector3 tl = terrain->GetPosAtDiscreteCoordinate(tl_idx);
			mb.SetUV(Vector2((float) x / terrain->m_dimensions.x, (float) (y + 1) / terrain->m_dimensions.y));
			mb.SetNormal(terrain->GetNormalAtDiscreteCoordinate(tl_idx));
			mb.SetTangent(Vector4(terrain->GetTangentAtDiscreteCoordinate(tl_idx), 1));
			mb.PushVertex(tl);

			IntVector2 tr_idx = IntVector2(MinInt(x + 1, terrain->m_dimensions.x - 1), MinInt(y + 1, terrain->m_dimensions.y - 1));
			Vector3 tr = terrain->GetPosAtDiscreteCoordinate(tr_idx);
			mb.SetUV(Vector2((float) (x + 1) / terrain->m_dimensions.x, (float) (y + 1) / terrain->m_dimensions.y));
			mb.SetNormal(terrain->GetNormalAtDiscreteCoordinate(tr_idx));
			mb.SetTangent(Vector4(terrain->GetTangentAtDiscreteCoordinate(tr_idx), 1));
			mb.PushVertex(tr);

			mb.AddQuad(index + 0, index + 1, index + 2, index + 3);
		}
	}
	mb.End();

	Mesh* mesh = new Mesh();
	mesh->FromBuilderForType<VertexLit>(mb);

	Vector2 center = m_terrain->GetChunkExtents(chunkIndex).GetCenter();
	m_transform.GetLocalPosition() = Vector3(center.x, m_terrain->GetHeight(center), center.y);
	m_renderable = new Renderable(mesh, &m_transform, mat);
	RenderScene::GetCurrentScene()->AddRenderable(m_renderable);
}

void TerrainChunk::CleanUp()
{
}
