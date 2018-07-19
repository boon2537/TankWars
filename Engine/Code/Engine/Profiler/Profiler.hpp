#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/PageAllocator.hpp"
#include "Engine/Core/Command.hpp"
#include <stdint.h>

#define PROFILE_MAX_HISTORY_LENGTH 256

struct profile_measurement_t 
{
	char m_id[64]; 
	uint64_t m_start_hpc;
	uint64_t m_end_hpc; 

	void AddChild(profile_measurement_t* child) 
	{
		if (m_children == nullptr)
		{
			m_children = child;
		}
		else
		{
			child->next = m_children;
			m_children = child;
		}

		m_children->m_parent = this;
	}

	void Finish()
	{
		m_end_hpc = GetPerformanceCounter();
	}

	inline uint64_t GetElapsedTime() { return m_end_hpc - m_start_hpc; }

	profile_measurement_t* m_parent = nullptr; 
	profile_measurement_t* m_children = nullptr; 
	profile_measurement_t* next = nullptr; 
};

class Profiler
{
public:
	~Profiler();
	Profiler();

	void ProfilePush(const char* tag); 
	void ProfilePop(); 
	void MarkFrame();
	profile_measurement_t* CreateMeasurement(const char* id);
	profile_measurement_t* ProfileGetPreviousFrame(uint skipCount = 0); 
	profile_measurement_t* ProfileGetWorstFrameInHistory();
	void DestroyMeasurementTreeRecursive(profile_measurement_t* node);
	void SaveReportFromFrame(profile_measurement_t* node);

	static Profiler* GetInstance();
	static void ProfilePause(Command& cmd);
	static void ProfileResume(Command& cmd);

public:
	profile_measurement_t* m_activeNode = nullptr; 
	profile_measurement_t* m_prevFrame = nullptr; 

	bool m_isPaused = false;
	bool m_pauseInitiated = false;
	int m_frameCount = 0;
	profile_measurement_t* m_frameHistory[PROFILE_MAX_HISTORY_LENGTH];
	int m_startIndex = 0;
	int m_endIndex = 0;

	TPageAllocator<profile_measurement_t> m_measurementPool;
};

void ProfilingSystemStartup();
void ProfilingSystemShutdown();
void ProfilerPush(const char* tag);
void ProfilerPop();
void ProfilerMarkFrame();
void ProfilerPause();
void ProfilerResume(); 

//////////////////////////////////////////////////////////////////////////
#define PROFILE_SCOPE(tag) ProfileScoped __timer_ ##__LINE__ ## (tag)
#define PROFILE_SCOPE_FUNCTION()  ProfileScoped __timer_ ##__LINE__ ## (__FUNCTION__)

class ProfileScoped
{
public:
	~ProfileScoped() 
	{
		ProfilerPop();
	}

	ProfileScoped(const char* tag)
	{
		ProfilerPush(tag);
	}
};
