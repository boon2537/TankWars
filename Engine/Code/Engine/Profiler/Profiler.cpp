#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Profiler/ProfilerView.hpp"

static Profiler* g_profiler = nullptr; 

Profiler::~Profiler()
{
	m_measurementPool.Clear();
}

Profiler::Profiler()
{
	for (int i = 0; i < PROFILE_MAX_HISTORY_LENGTH; i++)
		m_frameHistory[i] = nullptr;

	CommandRegister("profiler_pause", Profiler::ProfilePause, "Pause profiler");
	CommandRegister("profiler_resume", Profiler::ProfileResume, "Resume profiler");
}

void Profiler::ProfilePush(const char* tag)
{
	if (m_isPaused)
		return;

	profile_measurement_t* measurement = CreateMeasurement(tag);

	if (m_activeNode == nullptr) 
	{
		m_activeNode = measurement; 
	} 
	else 
	{
		measurement->m_parent = m_activeNode; 
		m_activeNode->AddChild(measurement); 
		m_activeNode = measurement; 
	}
}

void Profiler::ProfilePop()
{
	if (m_isPaused)
		return;

	ASSERT_OR_DIE(m_activeNode != nullptr, "Pop was called without a push");
	
	m_activeNode->Finish(); // sets end time 
	m_activeNode = m_activeNode->m_parent; 
}

void Profiler::MarkFrame()
{
	if (m_activeNode != nullptr) 
	{
		if (m_prevFrame != nullptr) 
		{
			SaveReportFromFrame(m_prevFrame);
		}

		m_prevFrame = m_activeNode;
		ProfilePop(); //pop "frame"

		ASSERT_OR_DIE(m_activeNode == nullptr, "Someone forgot to pop");
	}

	if (m_pauseInitiated)
		m_isPaused = true;
	else
		m_isPaused = false;

	ProfilePush("frame"); 
}

profile_measurement_t* Profiler::CreateMeasurement(const char* id)
{
	profile_measurement_t* measure = m_measurementPool.Create();
	measure->m_start_hpc = GetPerformanceCounter();
	strncpy_s(measure->m_id, id, 64);

	return measure; 
}

profile_measurement_t* Profiler::ProfileGetPreviousFrame(uint skipCount)
{
	if (skipCount > PROFILE_MAX_HISTORY_LENGTH - 1)
	{
		ASSERT_RECOVERABLE(false, "Inquired more than history length");
		return nullptr;
	}

	int index = m_frameCount % PROFILE_MAX_HISTORY_LENGTH;

	index -= skipCount + 1;
	if (index < 0)
		index = PROFILE_MAX_HISTORY_LENGTH + index;

	return m_frameHistory[index];
}

profile_measurement_t* Profiler::ProfileGetWorstFrameInHistory()
{
	profile_measurement_t* potential = nullptr;
	uint64_t frameTime = 0;

	for (int i = 0; i < PROFILE_MAX_HISTORY_LENGTH; i++)
	{
		if (m_frameHistory[i] != nullptr && m_frameHistory[i]->GetElapsedTime() > frameTime)
		{
			frameTime = m_frameHistory[i]->GetElapsedTime();
			potential = m_frameHistory[i];
		}
	}

	return potential;
}

void Profiler::DestroyMeasurementTreeRecursive(profile_measurement_t* node)
{
	if (node == nullptr)
		return;

	//delete all children
	profile_measurement_t* child = node->m_children;
	if (child != nullptr)
	{
		DestroyMeasurementTreeRecursive(child);
		while (child->next != nullptr)
		{
			child = child->next;
			DestroyMeasurementTreeRecursive(child);
		}
	}

	//delete yourself
	m_measurementPool.Destroy(node);
}

void Profiler::SaveReportFromFrame(profile_measurement_t* node)
{
	int index = m_frameCount % PROFILE_MAX_HISTORY_LENGTH;

	//destroy overwritten frame
	if (m_frameHistory[index] != nullptr)
		DestroyMeasurementTreeRecursive(m_frameHistory[index]);

	m_frameHistory[index] = node;
	m_frameCount++;
}


Profiler* Profiler::GetInstance()
{
	return g_profiler;
}

void Profiler::ProfilePause(Command& cmd)
{
	UNUSED(cmd);
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler->m_pauseInitiated = true;
#endif
}

void Profiler::ProfileResume(Command& cmd)
{
	UNUSED(cmd);
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler->m_pauseInitiated = false;
#endif
}

void ProfilingSystemStartup()
{
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler = new Profiler();

	//initialize other profiler features
	ProfilerReport::CreateInstance();
	ProfilerView::CreateInstance();
#endif
}

void ProfilingSystemShutdown()
{
#if !defined(ENGINE_DISABLE_PROFILING)
	delete g_profiler;
	g_profiler = nullptr;
#endif
}

void ProfilerPush(const char* tag)
{
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler->ProfilePush(tag);
#endif
}

void ProfilerPop()
{
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler->ProfilePop();
#endif
}

void ProfilerMarkFrame()
{
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler->MarkFrame();
#endif
}

void ProfilerPause()
{
#if !defined(ENGINE_DISABLE_PROFILING) 
	g_profiler->m_pauseInitiated = true;
#endif
}

void ProfilerResume()
{
#if !defined(ENGINE_DISABLE_PROFILING)
	g_profiler->m_pauseInitiated = false;
#endif
}
