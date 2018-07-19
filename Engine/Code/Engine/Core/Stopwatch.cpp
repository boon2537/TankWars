#include "Engine\Core\Stopwatch.hpp"
#include "Engine\Core\Time.hpp"

StopWatch::StopWatch(Clock* refClock)
{
	SetClock(refClock);
	Reset();
}

void StopWatch::SetClock(Clock* refClock)
{
	// maintain elapsed time 
	if (m_reference != nullptr)
	{
		m_cachedElapsedHPC += m_reference->m_total.hpc - m_start_HPC;
		m_start_HPC = m_reference->m_total.hpc;
	}

	m_reference = refClock;

	if (refClock == nullptr)
		m_reference = GetMasterClock();
}

void StopWatch::SetTimer(float seconds)
{
	m_start_HPC = m_reference->m_total.hpc;
	m_cachedElapsedHPC = 0;
	m_deductHPC = 0;
	m_intervalHPC = ConvertSecondsToPerformanceCounter(seconds);
}

float StopWatch::GetElapsedTime()
{
	return static_cast<float>(PerformanceCounterToSeconds(GetElapsedHPC()));
}

uint64_t StopWatch::GetElapsedHPC()
{
	if (m_isPaused)
	{
		m_deductHPC += m_reference->m_total.hpc - m_pauseRefHPC;
		m_pauseRefHPC = m_reference->m_total.hpc;
	}

	return m_reference->m_total.hpc - m_start_HPC + m_cachedElapsedHPC - m_deductHPC;
}

float StopWatch::GetNormalizedElapsedTime()
{
	if (m_intervalHPC == 0)
		return 1;

	return static_cast<float>(GetElapsedHPC()) / m_intervalHPC;
}

bool StopWatch::HasElapsed()
{
	return GetElapsedHPC() > m_intervalHPC;
}

void StopWatch::Reset()
{
	if (m_reference != nullptr)
	{
		m_start_HPC = m_reference->m_total.hpc;
		m_cachedElapsedHPC = 0;
		m_deductHPC = 0;
	}
}

bool StopWatch::CheckAndReset()
{
	bool hasElapsed = HasElapsed();
	if (hasElapsed)
	{
		Reset();
	}

	return hasElapsed;
}

bool StopWatch::Decrement()
{
	bool hasElapsed = HasElapsed();

	if (hasElapsed)
	{
		m_deductHPC += m_intervalHPC;
	}

	return hasElapsed;
}

unsigned int StopWatch::DecrementAll()
{
	if (HasElapsed())
	{
		uint64_t intervalsCompleted = GetElapsedHPC() / m_intervalHPC;
		m_deductHPC += m_intervalHPC * intervalsCompleted;
		return (unsigned int) intervalsCompleted;
	}
	
	return 0;
}

void StopWatch::Pause()
{
	m_isPaused = true;
	m_pauseRefHPC = m_reference->m_total.hpc;
}

void StopWatch::Resume()
{
	m_isPaused = false;
}
