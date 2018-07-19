#include "Engine/Core/Clock.hpp"

static Clock* g_masterClock = nullptr;

Clock::Clock(Clock* parent)
{
	m_parent = nullptr;

	Reset();
	m_scale = 1.0;
	m_paused = false;

	if (parent != nullptr)
	{
		parent->AddChild(this);
	}
}

void Clock::BeginFrame()
{
	uint64_t currentHPC = GetPerformanceCounter();
	uint64_t elapsed = currentHPC - m_lastFrameHPC;

	Advance(elapsed);
	m_lastFrameHPC = currentHPC;
}

void Clock::Reset()
{
	m_lastFrameHPC = GetPerformanceCounter();
	memset(&m_frame, 0, sizeof(m_frame)); //lazy way to set everything to 0
	memset(&m_total, 0, sizeof(m_total));
	m_frameCount = 0;
}

void Clock::Advance(uint64_t elapsed)
{
	m_frameCount++;

	if (m_paused)
	{
		elapsed = 0;
	}
	else
	{
		elapsed = (uint64_t) ((double) elapsed * m_scale);
	}

	double elapsedSeconds = PerformanceCounterToSeconds(elapsed);
	m_frame.seconds = elapsedSeconds;
	m_frame.hpc = elapsed;
	m_frame.ms = (unsigned int) m_frame.seconds * 1000;

	m_total.seconds += m_frame.seconds;
	m_total.hpc += m_frame.hpc;
	m_total.ms += m_frame.ms;

	m_lastFrameHPC = m_total.hpc;

	for (Clock* iter : m_children)
	{
		iter->Advance(elapsed);
	}
}

void Clock::AddChild(Clock* child)
{
	m_children.push_back(child);
}

double Clock::GetClockCurrentTime() const
{
	return m_total.seconds;
}

void Clock::SetPaused(bool paused)
{
	m_paused = paused;
}

void Clock::SetScale(float scale)
{
	m_scale = scale;
}

Clock* GetMasterClock()
{
	return g_masterClock;
}

void ClockSystemStartup()
{
	if (g_masterClock == nullptr)
	{
		g_masterClock = new Clock();
	}
	else
		g_masterClock->Reset();
}

void ClockSystemBeginFrame()
{
	g_masterClock->BeginFrame();
}

double GetSystemCurrentTime()
{
	return g_masterClock->m_total.seconds;
}

double GetSystemDeltaTime()
{
	return g_masterClock->m_frame.seconds;
}
