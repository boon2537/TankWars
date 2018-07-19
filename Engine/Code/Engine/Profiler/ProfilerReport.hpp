#pragma once

#include "Engine/Profiler/ProfilerReportEntry.hpp"

typedef std::pair<std::string, ProfilerReportEntry*> entryPair;

class ProfilerReport 
{
public:
	~ProfilerReport();
	ProfilerReport();

	void GenerateReportTreeFromFrame(profile_measurement_t *root);
	void GenerateReportFlatFromFrame(profile_measurement_t* root);
	void SortBySelfTime(ProfilerReportEntry* entry);
	void SortByTotalTime(ProfilerReportEntry* entry);
	double GetTotalFrameTime();

	void DestroyReportEntryRecursive(ProfilerReportEntry* entry);

	static ProfilerReport* CreateInstance();
	static ProfilerReport* GetInstance();
	static void ProfilerReportToConsole(Command& cmd);

public:
	//just a fake root to hold stuff
	ProfilerReportEntry* m_root = nullptr; 
	ProfilerReportEntry* m_frameRoot = nullptr;
};
