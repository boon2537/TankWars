#include "Game/FlockBehavior.hpp"
#include "Game/Enemy.hpp"

Vector2 FlockBehavior::CalculateSeekForce(const Vector2& target, Enemy* appliedEnemy)
{
	Vector2 desired = target - appliedEnemy->Get_XY_Pos();
	desired.NormalizeAndGetLength();
	desired = desired * appliedEnemy->m_maxSpeed;

	Vector2 steer = desired - appliedEnemy->m_velocity;
	steer.x = MinFloat(steer.x, appliedEnemy->m_maxForce);
	steer.y = MinFloat(steer.y, appliedEnemy->m_maxForce);
	return steer;
}

Vector2 FlockBehavior::CalculateSeperateForce(const std::vector<Enemy*>& enemies, Enemy* appliedEnemy)
{
	float desiredSeparation = appliedEnemy->m_radius * 2;
	Vector2 sum = Vector2::zero;

	int count = 0;
	for each (Enemy* e in enemies)
	{
		if (e != nullptr)
		{
			Vector2 otherPos = e->Get_XY_Pos();
			Vector2 myPos = appliedEnemy->Get_XY_Pos();
			float d = GetDistance(myPos, otherPos);
			if (d > 0 && d < desiredSeparation)
			{
				Vector2 diff = myPos - otherPos;
				diff.NormalizeAndGetLength();
				diff = diff / d;
				sum += diff;
				count++;
			}
		}
	}

	Vector2 steer = Vector2::zero;
	if (count > 0)
	{
		sum = sum / count;
		sum.NormalizeAndGetLength();
		sum *= appliedEnemy->m_maxSpeed;
		steer = sum - appliedEnemy->m_velocity;
		steer.x = MinFloat(steer.x, appliedEnemy->m_maxForce);
		steer.y = MinFloat(steer.y, appliedEnemy->m_maxForce);
	}

	return steer;
}

Vector2 FlockBehavior::CalculateAlignmentForce(const std::vector<Enemy*>& enemies, Enemy* appliedEnemy)
{
	Vector2 sum = Vector2::zero;
	Vector2 myPos = Vector2(appliedEnemy->m_transform.GetWorldPosition().x, appliedEnemy->m_transform.GetWorldPosition().z);
	int count = 0;
	for each (Enemy* e in enemies)
	{
		if (e != nullptr)
		{
			float d = GetDistance(myPos, e->Get_XY_Pos());
			if (d > 0 && d < m_alignmentNeighborDist)
			{
				sum += e->m_velocity;
				count++;
			}
		}
	}

	if (count > 0)
	{
		sum = sum / count;
		sum.NormalizeAndGetLength();
		sum *= appliedEnemy->m_maxSpeed;

		Vector2 steer = sum - appliedEnemy->m_velocity;
		steer.x = MinFloat(steer.x, appliedEnemy->m_maxForce);
		steer.y = MinFloat(steer.y, appliedEnemy->m_maxForce);
		return steer;
	}
	else
	{
		return Vector2::zero;
	}
}

Vector2 FlockBehavior::CalculateCohesionForce(const std::vector<Enemy*>& enemies, Enemy* appliedEnemy)
{
	Vector2 sum = Vector2::zero;
	int count = 0;
	for each (Enemy* e in enemies)
	{
		float d = GetDistance(appliedEnemy->Get_XY_Pos(), e->Get_XY_Pos());
		if (d > 0 && d < m_cohesionNeighborDist)
		{
			sum += e->Get_XY_Pos();
			count++;
		}
	}

	if (count > 0)
	{
		sum = sum / count;
		return CalculateSeekForce(sum, appliedEnemy);
	}
	else
	{
		return Vector2::zero;
	}
}
