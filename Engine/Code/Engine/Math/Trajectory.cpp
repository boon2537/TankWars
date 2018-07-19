#include "Engine/Math/Trajectory.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

Vector2 Trajectory::Evaluate(float gravity, Vector2 launchVelocity, float time)
{
	return Vector2(launchVelocity.x * time, (-0.5f * gravity * time * time) + (launchVelocity.y * time));
}

Vector2 Trajectory::Evaluate(float gravity, float launchSpeed, float launchAngle, float time)
{
	return Vector2(launchSpeed * cosf(launchAngle) * time, (-0.5f * gravity * time * time) + (launchSpeed * sinf(launchAngle) * time));
}

float Trajectory::GetMinimunLaunchSpeed(float gravity, float distance)
{
	return sqrtf(gravity * distance);
}

bool Trajectory::GetLaunchAngles(Vector2* out, float gravity, float launchSpeed, float distance, float height)
{
	float insideRoot = ((launchSpeed * launchSpeed * launchSpeed * launchSpeed) - (gravity * ((gravity * distance * distance) + (2 * height * launchSpeed * launchSpeed))));
	if (insideRoot < 0)
		return false;

	//reference https://gamedev.stackexchange.com/questions/53552/how-can-i-find-a-projectiles-launch-angle
	float firstAngle = atan2f((launchSpeed * launchSpeed) - sqrtf(insideRoot), (gravity * distance));
	float secondAngle = atan2f((launchSpeed * launchSpeed) + sqrtf(insideRoot), (gravity * distance));

	out->x = firstAngle;
	out->y = secondAngle;
	return true;
}

float Trajectory::GetMaxHeight(float gravity, float launchSpeed, float distance)
{
	float angle = atan2f(launchSpeed * launchSpeed, gravity * distance);

	float Vx = launchSpeed * cosf(angle);
	float Vy = launchSpeed * sinf(angle);
	float time = distance / Vx;

	float height = (-0.5f * gravity * time * time) + (Vy * time);
	return height;
}

Vector2 Trajectory::GetLaunchVelocity(float gravity, float apexHeight, float distance, float height, float* out_timeAirborne)
{
	float Vy = sqrtf(2 * gravity * apexHeight);

	float timeToApex = Vy / gravity;
	float timeToFall = sqrtf((2 * (apexHeight - height)) / gravity);
	float timeAirborne = timeToApex + timeToFall;
	
	if (out_timeAirborne != nullptr)
		*out_timeAirborne = timeAirborne;

	float Vx = distance / timeAirborne;
	//float Vy = ((apexHeight - height) / timeAirborne) + (0.5f * gravity * timeAirborne);
	return Vector2(Vx, Vy);
}
