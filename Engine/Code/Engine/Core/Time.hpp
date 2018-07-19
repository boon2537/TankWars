//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once

#include <stdint.h>
#include <string>

//Legacy support.....
//TODO: remove later
double GetCurrentTimeSeconds();
std::string GetFormatedDateTime();

uint64_t GetPerformanceCounter();
double PerformanceCounterToSeconds(const uint64_t& hpc);
uint64_t ConvertSecondsToPerformanceCounter(float seconds);
std::string TimePerfCountToString(const uint64_t& hpc);
