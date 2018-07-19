#pragma once

#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include <stdint.h>

#define PROFILE_LOG_SCOPE(tag) ProfileLogScoped __timer_ ##__LINE__ ## (tag)
#define PROFILE_LOG_SCOPE_FUNCTION()  ProfileLogScoped __timer_ ##__LINE__ ## (__FUNCTION__)

class ProfileLogScoped
{
public:
	~ProfileLogScoped() 
	{
		uint64_t deltaHPC = GetPerformanceCounter() - m_hpc_start;
		std::string timeText = TimePerfCountToString(deltaHPC);
		DebuggerPrintf("%s took %s (%llu hpc)", m_tag, timeText.c_str(), deltaHPC);
		DebugLogf(Stringf("%s took %s (%llu hpc)", m_tag, timeText.c_str(), deltaHPC), Rgba::yellow, 2.f);
	}

	ProfileLogScoped(const char* tag)
		: m_tag(tag)
	{
		m_hpc_start = GetPerformanceCounter();
	}

public:
	uint64_t m_hpc_start; 
	const char* m_tag;        // don't worry about storage - this is stack based, 
							// so any pointer we pass in will still be in scope; 
};
