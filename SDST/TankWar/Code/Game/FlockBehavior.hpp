#pragma once

#include "Engine/Math/Vector2.hpp"
#include <vector>

class Enemy;

class FlockBehavior
{
public:
	~FlockBehavior() {};
	FlockBehavior() {};

	Vector2 CalculateSeekForce(const Vector2& target, Enemy* appliedEnemy);
	Vector2 CalculateSeperateForce(const std::vector<Enemy*>& enemies, Enemy* appliedEnemy);
	Vector2 CalculateAlignmentForce(const std::vector<Enemy*>& enemies, Enemy* appliedEnemy);
	Vector2 CalculateCohesionForce(const std::vector<Enemy*>& enemies, Enemy* appliedEnemy);

public:
	float m_seekForceWeight = 3.5f;
	float m_seperateForceWeight = 1.f;
	float m_alighmentForceWeight = 0.1f;
	float m_cohesionForceWeight = 0.04f;
	float m_alignmentNeighborDist = 50.f;
	float m_cohesionNeighborDist = 25.f;
};
