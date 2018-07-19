#include "Engine/Core/HeatMap.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

HeatMap::HeatMap(IntVector2 dimensions, float initValue)
	: m_dimensions(dimensions)
{
	for (int y = 0; y < dimensions.y; y++)
	{
		for (int x = 0; x < dimensions.x; x++)
			m_heatPerGridCell.push_back(initValue);
	}
}

void HeatMap::SetHeat(const IntVector2& cellCoords, float setValue)
{
	unsigned int index = cellCoords.x + (cellCoords.y * m_dimensions.x);
	if (index < m_heatPerGridCell.size())
	{
		m_heatPerGridCell[index] = setValue;
		return;
	}

	ERROR_RECOVERABLE("cellCoords out of range");
}

void HeatMap::AddHeat(const IntVector2& cellCoords, float addValue)
{
	unsigned int index = cellCoords.x + (cellCoords.y * m_dimensions.x);
	if (index < m_heatPerGridCell.size())
	{
		m_heatPerGridCell[index] += addValue;
		return;
	}

	ERROR_RECOVERABLE("cellCoords out of range");
}

float HeatMap::GetHeat(const IntVector2& cellCoords) const
{
	unsigned int index = cellCoords.x + (cellCoords.y * m_dimensions.x);
	if (index < m_heatPerGridCell.size())
		return m_heatPerGridCell[index];

	ERROR_AND_DIE("cellCoords out of range");
}

void HeatMap::InitHeat(float initValue)
{
	for (int y = 0; y < m_dimensions.y; y++)
	{
		for (int x = 0; x < m_dimensions.x; x++)
			SetHeat(IntVector2(x, y), initValue);
	}
}

IntVector2 HeatMap::GetDimensions()
{
	return m_dimensions;
}
