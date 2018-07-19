#pragma once

#include "Engine/Math/IntVector2.hpp"
#include <vector>

class HeatMap
{
public:
	~HeatMap() {};
	HeatMap() {};
	HeatMap(IntVector2 dimensions, float initValue);

	void SetHeat(const IntVector2& cellCoords, float setValue);
	void AddHeat(const IntVector2& cellCoords, float addValue);
	float GetHeat(const IntVector2& cellCoords) const;
	void InitHeat(float initValue);

	IntVector2 GetDimensions();

private:
	std::vector<float> m_heatPerGridCell;
	IntVector2 m_dimensions;
};
