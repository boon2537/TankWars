#include "Engine/Debug/DebugRenderTask.hpp"
#include "Engine/Math/MathUtils.hpp"

DebugRenderTask::~DebugRenderTask()
{
}

DebugRenderTask::DebugRenderTask()
{
}

void DebugRenderTask::Age(float deltaSeconds)
{
	m_timeToLive += deltaSeconds;
}

float DebugRenderTask::GetNormalizedAge()
{
	float normalizeAge = 0.0f;
	float age = m_options.totalTimeToLive - m_timeToLive;
	if (m_options.totalTimeToLive > 0.0f)
	{
		normalizeAge = age / m_options.totalTimeToLive;
	}
	return ClampFloat(normalizeAge, 0.f, 1.f);
}
