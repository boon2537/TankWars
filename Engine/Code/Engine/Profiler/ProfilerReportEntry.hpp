#pragma once

#include "Engine/Profiler/Profiler.hpp"
#include <vector>

class ProfilerReportEntry 
{
public:
	~ProfilerReportEntry();
	ProfilerReportEntry(const char* id);

	void PopulateTree(profile_measurement_t* node) ;
	void AccumulateData(profile_measurement_t* node);
	void PopulateFlat(profile_measurement_t* node);
	void Finalize(ProfilerReportEntry* entry, uint64_t frameHPC); //recursively finalize nodes (calc percents)
	ProfilerReportEntry* FindEntry(const char* str);
	ProfilerReportEntry* GetOrCreateChild(const char* str);
	double GetTotalElapsedTime();

	uint64_t GetChildrenTime(profile_measurement_t* node);

public:
	std::string m_id = ""; 
	uint64_t m_callCount = 0; 
	uint64_t m_totalHPC = 0; // inclusive time; 
	uint64_t m_selfHPC = 0;  // exclusive time
	double m_totalPercent = 0.0;
	double m_selfPercent = 0.0;

	std::string m_selfPercentStr = "0.00%";
	std::string m_totalPercentStr = "0.00%";

	ProfilerReportEntry* m_parent = nullptr; 
	std::map<std::string, ProfilerReportEntry*> m_children; 

	std::vector<std::pair<std::string, ProfilerReportEntry*>> m_sortedChildren;
};
