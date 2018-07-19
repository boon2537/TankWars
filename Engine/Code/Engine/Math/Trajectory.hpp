#pragma once

#include "Engine/Math//Vector2.hpp"
class Trajectory
{
public:
	static Vector2 Evaluate(float gravity, Vector2 launchVelocity, float time);
	static Vector2 Evaluate(float gravity, float launchSpeed, float launchAngle, float time);
	
	static float GetMinimunLaunchSpeed(float gravity, float distance); //assume firing angle of 45 degrees

	static bool GetLaunchAngles(Vector2* out,
		float gravity,          // gravity 
		float launchSpeed,		// launch speed
		float distance,         // horizontal displacement desired
		float height = 0.0f);   // vertical displacement desired

	static float GetMaxHeight(float gravity, float launchSpeed, float distance);

	//assume the apex height is positive and greater than the target height
	//return: The launch velocity needed to hit the apex height, and the distance.
	static Vector2 GetLaunchVelocity(float gravity,
		float apexHeight,       // must be greater than height
		float distance,         // target distance
		float height,			// target height
		float* out_timeAirborne = nullptr);         
};
