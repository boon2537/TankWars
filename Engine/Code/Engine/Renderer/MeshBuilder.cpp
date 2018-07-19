#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/File/File.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/ThirdParty/mikkt/mikktspace.h"
#include <iostream> 
#include <fstream>

MeshBuilder::~MeshBuilder()
{
}

MeshBuilder::MeshBuilder()
{
}

void MeshBuilder::Begin(eDrawPrimitive prim, bool use_indices)
{
	m_draw.m_primitiveType = prim; 
	m_draw.m_usingIndices = use_indices; 

	if (use_indices) {
		m_draw.m_startIndex = (uint) m_indices.size(); 
	} else {
		m_draw.m_startIndex = (uint) m_vertices.size(); 
	}
}

void MeshBuilder::End()
{
	uint endIndex;
	if (m_draw.m_usingIndices) {
		endIndex = (uint) m_indices.size(); 
	} else {
		endIndex = (uint) m_vertices.size(); 
	}

	m_draw.m_startIndex = 0;
	m_draw.m_elemCount = endIndex - m_draw.m_startIndex; 
}

void MeshBuilder::Reset()
{
	m_indices.clear();
	m_vertices.clear();
	End();
}

void MeshBuilder::SetNormal(const Vector3& normal)
{
	m_stamp.normal = normal;
}

void MeshBuilder::SetColor(const Rgba& color)
{
	m_stamp.color = color; 
}

void MeshBuilder::SetUV(const Vector2& uv)
{
	m_stamp.UV = uv; 
}

void MeshBuilder::SetTangent(const Vector4& tangent)
{
	m_stamp.tangent = tangent;
}

void MeshBuilder::GenerateArbitraryTangents(Vector3* tangent, Vector3* bitangent, Vector3 normal)
{
	Vector3 right = CrossProduct(normal, Vector3::up);
	if (right.GetLengthSquared() == 0)
	{
		right = CrossProduct(normal, Vector3::right);
	}
	right.NormalizeAndGetLength();

	Vector3 up = CrossProduct(right, normal);
	up.NormalizeAndGetLength();

	*tangent = right;

	if (bitangent != nullptr)
		*bitangent = up;
}

uint MeshBuilder::PushVertex(const Vector3& position)
{
	m_stamp.position = position; 
	m_vertices.push_back(m_stamp); 

	return (uint) m_vertices.size() - 1; 
}

void MeshBuilder::AddFace(uint first, uint second, uint third)
{
	m_indices.push_back(first);
	m_indices.push_back(second);
	m_indices.push_back(third);
}

void MeshBuilder::AddQuad(uint p0, uint p1, uint p2, uint p3)
{
	AddFace(p0, p1, p2);
	AddFace(p2, p1, p3);
}

void MeshBuilder::AddCube(const Vector3& center, const Vector3& dimensions, const Vector3& scale, const Rgba& tint, AABB2 uv_top, AABB2 uv_side, AABB2 uv_bottom)
{
	float x_min = (scale.x * (-1 * (dimensions.x / 2))) + center.x;
	float x_max = (scale.x * (dimensions.x / 2)) + center.x;
	float y_min = (scale.y * (-1 * (dimensions.y / 2))) + center.y;
	float y_max = (scale.y * (dimensions.y / 2)) + center.y;
	float z_min = (scale.z * (-1 * (dimensions.z / 2))) + center.z;
	float z_max = (scale.z * (dimensions.z / 2)) + center.z;

	Begin(eDrawPrimitive::TRIANGLES, true);
	SetColor(tint);

	//xy, z-
	SetNormal(Vector3(0, 0, -1));
	SetTangent(Vector4(1, 0, 0, 1));
	SetUV(Vector2(uv_side.maxs.x, uv_side.mins.y));
	uint index = PushVertex(Vector3(x_max, y_min, z_min));
	SetUV(Vector2(uv_side.mins.x, uv_side.mins.y));
	PushVertex(Vector3(x_min, y_min, z_min));
	SetUV(Vector2(uv_side.maxs.x, uv_side.maxs.y));
	PushVertex(Vector3(x_max, y_max, z_min));
	SetUV(Vector2(uv_side.mins.x, uv_side.maxs.y));
	PushVertex(Vector3(x_min, y_max, z_min));

	AddFace(index + 3, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 0);

	//xy, z+
	SetNormal(Vector3(0, 0, 1));
	SetTangent(Vector4(-1, 0, 0, 1));
	SetUV(Vector2(uv_side.mins.x, uv_side.mins.y));
	index = PushVertex(Vector3(x_max, y_min, z_max));
	SetUV(Vector2(uv_side.maxs.x, uv_side.mins.y));
	PushVertex(Vector3(x_min, y_min, z_max));
	SetUV(Vector2(uv_side.mins.x, uv_side.maxs.y));
	PushVertex(Vector3(x_max, y_max, z_max));
	SetUV(Vector2(uv_side.maxs.x, uv_side.maxs.y));
	PushVertex(Vector3(x_min, y_max, z_max));

	AddFace(index + 0, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 3);

	//xz, y- bot
	SetNormal(Vector3(0, -1, 0));
	SetTangent(Vector4(1, 0, 0, 1));
	SetUV(Vector2(uv_bottom.mins.x, uv_bottom.mins.y));
	index = PushVertex(Vector3(x_max, y_min, z_min));
	SetUV(Vector2(uv_bottom.maxs.x, uv_bottom.mins.y));
	PushVertex(Vector3(x_min, y_min, z_min));
	SetUV(Vector2(uv_bottom.mins.x, uv_bottom.maxs.y));
	PushVertex(Vector3(x_max, y_min, z_max));
	SetUV(Vector2(uv_bottom.maxs.x, uv_bottom.maxs.y));
	PushVertex(Vector3(x_min, y_min, z_max));

	AddFace(index + 0, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 3);

	//yz, x+
	SetNormal(Vector3(1, 0, 0));
	SetTangent(Vector4(0, 0, 1, 1));
	SetUV(Vector2(uv_side.maxs.x, uv_side.mins.y));
	index = PushVertex(Vector3(x_max, y_min, z_max));
	SetUV(Vector2(uv_side.mins.x, uv_side.mins.y));
	PushVertex(Vector3(x_max, y_min, z_min));
	SetUV(Vector2(uv_side.maxs.x, uv_side.maxs.y));
	PushVertex(Vector3(x_max, y_max, z_max));
	SetUV(Vector2(uv_side.mins.x, uv_side.maxs.y));
	PushVertex(Vector3(x_max, y_max, z_min));

	AddFace(index + 3, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 0);

	//yz, x-
	SetNormal(Vector3(-1, 0, 0));
	SetTangent(Vector4(0, 0, -1, 1));
	SetUV(Vector2(uv_side.mins.x, uv_side.mins.y));
	index = PushVertex(Vector3(x_min, y_min, z_max));
	SetUV(Vector2(uv_side.maxs.x, uv_side.mins.y));
	PushVertex(Vector3(x_min, y_min, z_min));
	SetUV(Vector2(uv_side.mins.x, uv_side.maxs.y));
	PushVertex(Vector3(x_min, y_max, z_max));
	SetUV(Vector2(uv_side.maxs.x, uv_side.maxs.y));
	PushVertex(Vector3(x_min, y_max, z_min));

	AddFace(index + 0, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 3);

	//xz, y+ top
	SetNormal(Vector3(0, 1, 0));
	SetTangent(Vector4(1, 0, 0, 1));
	SetUV(Vector2(uv_top.maxs.x, uv_top.mins.y));
	index = PushVertex(Vector3(x_max, y_max, z_min));
	SetUV(Vector2(uv_top.mins.x, uv_top.mins.y));
	PushVertex(Vector3(x_min, y_max, z_min));
	SetUV(Vector2(uv_top.maxs.x, uv_top.maxs.y));
	PushVertex(Vector3(x_max, y_max, z_max));
	SetUV(Vector2(uv_top.mins.x, uv_top.maxs.y));
	PushVertex(Vector3(x_min, y_max, z_max));

	AddFace(index + 3, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 0);

	End();
}

void MeshBuilder::AddUVSphere(const Vector3& position, float radius, uint wedges, uint slices, const Rgba& tint)
{
	Begin(eDrawPrimitive::TRIANGLES, true);
	SetColor(tint);

	// (0,0) to (1,1)
	for (uint slice_idx = 0; slice_idx <= slices; ++slice_idx) 
	{
		float v = (float)(slice_idx) / (float)(slices);
		float azimuth = RangeMapFloat( v, 0, 1, -MathUtils::PI/2.0f, MathUtils::PI/2.0f ); 

		for (uint wedge_idx = 0; wedge_idx < wedges; ++wedge_idx) 
		{
			float u = (float)wedge_idx / (float)(wedges - 1); 
			float rot = 2.0f * MathUtils::PI * u; 

			SetUV(Vector2(u, v));
			Vector3 pos = position + PolarToCartesian(radius, rot, azimuth); 
			SetNormal((pos - position).GetNormalized());
			SetTangent(Vector4(-1.f * cosf(azimuth) * sinf(rot) * radius, 0, cosf(azimuth) * cosf(rot) * radius, 1));
			PushVertex(pos);
		}
	}

	for (uint slice_idx = 0; slice_idx < slices; ++slice_idx) 
	{
		for (uint wedge_idx = 0; wedge_idx < wedges; ++wedge_idx) 
		{	
			uint bl_idx = wedges * slice_idx + wedge_idx; 
			uint tl_idx = bl_idx + wedges; 
			uint br_idx = bl_idx + 1; 
			uint tr_idx = br_idx + wedges;

			if (tr_idx < m_vertices.size())
				AddQuad(bl_idx, br_idx, tl_idx, tr_idx); 
		}
	}

	End();
}

void MeshBuilder::AddPlane(const Vector3& center, const Vector3& rightVec, const Vector3& upVec, const AABB2& bounds, AABB2 uv, const Rgba& tint)
{
	Vector2 dimensions = bounds.GetDimensions();
	float left = -1 * 0.5f * dimensions.x;
	float right = left + dimensions.x;
	float bottom = -1 * 0.5f * dimensions.y;
	float top = bottom + dimensions.y;

	Vector3 p0 = center + (rightVec * left) + (upVec * bottom);
	Vector3 p1 = center + (rightVec * right) + (upVec * bottom);
	Vector3 p2 = center + (rightVec * left) + (upVec * top);
	Vector3 p3 = center + (rightVec * right) + (upVec * top);

	Begin(eDrawPrimitive::TRIANGLES, true);
	SetColor(tint);

	//xy, z-
	SetNormal(Vector3(0, 0, -1));
	SetTangent(Vector4(1, 0, 0, 1));
	SetUV(Vector2(uv.maxs.x, uv.mins.y));
	uint index = PushVertex(p1);
	SetUV(Vector2(uv.mins.x, uv.mins.y));
	PushVertex(p0);
	SetUV(Vector2(uv.maxs.x, uv.maxs.y));
	PushVertex(p3);
	SetUV(Vector2(uv.mins.x, uv.maxs.y));
	PushVertex(p2);

	AddFace(index + 3, index + 1, index + 2);
	AddFace(index + 2, index + 1, index + 0);

	End();
}

void MeshBuilder::AddSprite(const Vector3 & center, const Vector2 & extends, const Vector3 & scale, const Rgba & tint, AABB2 uv)
{
	float x_min = (scale.x * (-1 * (extends.x / 2))) + center.x;
	float x_max = (scale.x * (extends.x / 2)) + center.x;
	float y_min = (scale.y * (-1 * (extends.y / 2))) + center.y;
	float y_max = (scale.y * (extends.y / 2)) + center.y;
	float z = center.z;

	Begin(eDrawPrimitive::TRIANGLES, true);
	SetColor(tint);

	SetUV(Vector2(uv.mins.x, uv.maxs.y));
	uint index = PushVertex(Vector3(x_min, y_min, z));
	SetUV(Vector2(uv.maxs.x, uv.maxs.y));
	PushVertex(Vector3(x_max, y_min, z));
	SetUV(Vector2(uv.mins.x, uv.mins.y));
	PushVertex(Vector3(x_min, y_max, z));
	SetUV(Vector2(uv.maxs.x, uv.mins.y));
	PushVertex(Vector3(x_max, y_max, z));

	AddQuad(index, index + 1, index + 2, index + 3);
	End();
}

void MeshBuilder::AddFromObjFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file) 
	{
		ASSERT_RECOVERABLE(false, "Cannot find file at path " + path);
		return;
	}

	Begin(eDrawPrimitive::TRIANGLES, true);
	SetColor(Rgba::white);

	std::vector<Vector3> vertexs;
	std::vector<Vector3> normals;
	std::vector<Vector2> uvs;
	size_t startPos;

	std::string text;
	uint anchorIndex = 0;
	uint indexMB = 0;

	while (std::getline(file, text))
	{
		startPos = text.find("v ");
		if (startPos != std::string::npos)
		{
			text = std::string(text, startPos + 3);
			Vector3 vertex;
			vertex.SetFromText(text.c_str(), ' ');
			vertex.x *= -1; //flip
			vertexs.push_back(vertex);
		}

		startPos = text.find("vn ");
		if (startPos != std::string::npos)
		{
			text = std::string(text, startPos + 3);
			Vector3 normal;	
			normal.SetFromText(text.c_str(), ' ');
			normal.x *= -1; //flip
			normals.push_back(normal);
		}

		startPos = text.find("vt ");
		if (startPos != std::string::npos)
		{
			text = std::string(text, startPos + 3);
			Vector2 uv;		
			uv.SetFromText(text.c_str(), ' ');
			uvs.push_back(uv);
		}

		startPos = text.find("f ");
		if (startPos != std::string::npos)
		{
			text = std::string(text, startPos + 2);

			//delete trailing white space
			if (text[text.length() - 1] == ' ')
				text.erase(text.end() - 1);

			Strings splitted = Split(text, ' ');
			Strings indexes;
			Vector3 position;

			for (unsigned int i = 0; i < splitted.size(); i++)
			{
				indexes = Split(splitted[i], '/');
				for (unsigned int j = 0; j < indexes.size(); j++)
				{
					int indexToUse = atoi(indexes[j].c_str()) - 1;
					if (j == 0)
						position = vertexs[indexToUse];
					else if (j == 1)
						SetUV(uvs[indexToUse]);
					else if (j == 2)
					{
						SetNormal(normals[indexToUse]);

						//to be removed once put in MikT
						Vector3 tangent;
						GenerateArbitraryTangents(&tangent, nullptr, normals[indexToUse]);
						SetTangent(Vector4(tangent.x, tangent.y, tangent.z, 1));
					}
				}

				indexMB = PushVertex(position);
			}

			//inject mikT here, don't use index buffer...get rid of duplicate instead

			//quad
			if (splitted.size() == 4)
			{
				AddFace(anchorIndex + 0, anchorIndex + 1, anchorIndex + 2);
				AddFace(anchorIndex + 0, anchorIndex + 2, anchorIndex + 3);
			}			
			//triangle
			else if (splitted.size() == 3)
			{
				AddFace(anchorIndex + 0, anchorIndex + 1, anchorIndex + 2);
			}

			anchorIndex = indexMB + 1;
		}
	}

	End();
}
