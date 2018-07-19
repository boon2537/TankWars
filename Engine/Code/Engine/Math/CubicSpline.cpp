#include "Engine/Math/CubicSpline.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

CubicSpline2D::CubicSpline2D(const Vector2 * positionsArray, int numPoints, const Vector2 * velocitiesArray)
{
	AppendPoints(positionsArray, numPoints, velocitiesArray);
}

void CubicSpline2D::AppendPoint(const Vector2 & position, const Vector2 & velocity)
{
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}

void CubicSpline2D::AppendPoints(const Vector2 * positionsArray, int numPoints, const Vector2 * velocitiesArray)
{
	for (int i = 0; i < numPoints; i++)
	{
		m_positions.push_back(*positionsArray);
		positionsArray++;

		if (velocitiesArray != nullptr)
		{
			m_velocities.push_back(*velocitiesArray);
			velocitiesArray++;
		}
		else
		{
			m_velocities.push_back(Vector2::zero);
		}
	}
}

void CubicSpline2D::InsertPoint(int insertBeforeIndex, const Vector2 & position, const Vector2 & velocity)
{
	m_positions.insert(m_positions.begin() + insertBeforeIndex, position);
	m_velocities.insert(m_velocities.begin() + insertBeforeIndex, velocity);
}

void CubicSpline2D::RemovePoint(int pointIndex)
{
	m_positions.erase(m_positions.begin() + pointIndex);
	m_velocities.erase(m_velocities.begin() + pointIndex);
}

void CubicSpline2D::RemoveAllPoints()
{
	m_positions.clear();
	m_velocities.clear();
}

void CubicSpline2D::SetPoint(int pointIndex, const Vector2 & newPosition, const Vector2 & newVelocity)
{
	if ((unsigned int) pointIndex >= m_positions.size())
	{
		ERROR_RECOVERABLE("Point index out of range");
		return;
	}

	m_positions[pointIndex] = newPosition;
	m_velocities[pointIndex] = newVelocity;
}

void CubicSpline2D::SetPosition(int pointIndex, const Vector2 & newPosition)
{
	if ((unsigned int) pointIndex >= m_positions.size())
	{
		ERROR_RECOVERABLE("Point index out of range");
		return;
	}

	m_positions[pointIndex] = newPosition;
}

void CubicSpline2D::SetVelocity(int pointIndex, const Vector2 & newVelocity)
{
	if ((unsigned int) pointIndex >= m_velocities.size())
	{
		ERROR_RECOVERABLE("Point index out of range");
		return;
	}

	m_velocities[pointIndex] = newVelocity;
}

void CubicSpline2D::SetCardinalVelocities(float tension, const Vector2 & startVelocity, const Vector2 & endVelocity)
{
	//set end points
	if (m_positions.size() > 1)
	{
		m_velocities[0] = startVelocity;
		m_velocities[m_velocities.size() - 1] = endVelocity;
	}

	for (unsigned int i = 1; i < m_positions.size() - 1; i++)
	{
		m_velocities[i] = (m_positions[i + 1] - m_positions[i - 1]) * 0.5f * (1 - tension);
	}
}

const Vector2 CubicSpline2D::GetPosition(int pointIndex)
{
	if ((unsigned int) pointIndex >= m_positions.size())
	{
		ERROR_RECOVERABLE("Point index out of range");
		return Vector2::zero;
	}

	return m_positions[pointIndex];
}

const Vector2 CubicSpline2D::GetVelocity(int pointIndex)
{
	if ((unsigned int) pointIndex >= m_velocities.size())
	{
		ERROR_RECOVERABLE("Point index out of range");
		return Vector2::zero;
	}

	return m_velocities[pointIndex];
}

int CubicSpline2D::GetPositions(std::vector<Vector2>& out_positions) const
{
	out_positions = m_positions;
	return static_cast<int>(m_positions.size());
}

int CubicSpline2D::GetVelocities(std::vector<Vector2>& out_velocities) const
{
	out_velocities = m_velocities;
	return static_cast<int>(m_velocities.size());
}

Vector2 CubicSpline2D::EvaluateAtCumulativeParametric(float t) const
{
	int segment = (int) t;

	if ((unsigned int) segment > m_positions.size() - 1)
	{
		ERROR_RECOVERABLE("Segment out of range");
		return Vector2::zero;
	}

	return EvaluateCubicHermite(m_positions[segment], m_velocities[segment], m_positions[segment + 1], m_velocities[segment + 1], t - segment);
}

Vector2 CubicSpline2D::EvaluateAtNormalizedParametric(float t) const
{
	int segment = static_cast<int>(t * (m_positions.size() - 1));

	if ((unsigned int) segment >= m_positions.size() - 1)
	{
		ERROR_RECOVERABLE("Segment out of range");
		return Vector2::zero;
	}

	return EvaluateCubicHermite(m_positions[segment], m_velocities[segment], m_positions[segment + 1], m_velocities[segment + 1], (t * (m_positions.size() - 1)) - segment);
}
