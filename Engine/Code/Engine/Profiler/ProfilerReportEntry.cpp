#include "Engine/Profiler/ProfilerReportEntry.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/STL_Utils.hpp"

ProfilerReportEntry::~ProfilerReportEntry()
{
}

ProfilerReportEntry::ProfilerReportEntry(const char* id)
{
	m_id = id;
}

void ProfilerReportEntry::PopulateTree(profile_measurement_t* node) 
{
	AccumulateData(node); 

	//For each children...
	profile_measurement_t* child = node->m_children;
	if (child != nullptr)
	{
		ProfilerReportEntry* entry;
		do 
		{
			entry = GetOrCreateChild(child->m_id); 
			entry->PopulateTree(child); 
			child = child->next;
		}
		while (child != nullptr);
	}

	//tally up children time
	uint64_t childrenTime = 0;
	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++)
	{
		childrenTime += it->second->m_totalHPC;
	}
	m_selfHPC = m_totalHPC - childrenTime;
}

void ProfilerReportEntry::AccumulateData(profile_measurement_t* node) 
{
	m_callCount++; 
	m_totalHPC += node->GetElapsedTime();

	//inefficient but works
	m_selfHPC = m_totalHPC - GetChildrenTime(node);
}

void ProfilerReportEntry::PopulateFlat(profile_measurement_t* node) 
{
	//For each children...
	profile_measurement_t* child = node->m_children;
	if (child != nullptr)
	{
		ProfilerReportEntry* entry;
		do 
		{
			entry = GetOrCreateChild(child->m_id); 
			entry->AccumulateData(child); 
			PopulateFlat(child);
			child = child->next;
		} 
		while (child != nullptr);
	}
}

void ProfilerReportEntry::Finalize(ProfilerReportEntry* entry, uint64_t frameHPC)
{
	entry->m_totalPercent = PerformanceCounterToSeconds(entry->m_totalHPC) / PerformanceCounterToSeconds(frameHPC);
	entry->m_selfPercent = PerformanceCounterToSeconds(entry->m_selfHPC) / PerformanceCounterToSeconds(frameHPC);

	entry->m_totalPercentStr = Stringf("%.2f%s", entry->m_totalPercent * 100.0, "%");
	entry->m_selfPercentStr = Stringf("%.2f%s", entry->m_selfPercent * 100.0, "%");

	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = entry->m_children.begin(); it != entry->m_children.end(); it++)
	{
		Finalize(it->second, frameHPC);
	}
}

ProfilerReportEntry* ProfilerReportEntry::FindEntry(const char* str)
{
	return FindInMap<std::string,ProfilerReportEntry*>(m_children, str);
}

ProfilerReportEntry* ProfilerReportEntry::GetOrCreateChild(const char* str) 
{
	ProfilerReportEntry* entry = FindEntry(str); 
	if (entry == nullptr) 
	{
		entry = new ProfilerReportEntry(str); 
		entry->m_parent = this; 
		m_children[entry->m_id] = entry; 
	}

	return entry; 
}

double ProfilerReportEntry::GetTotalElapsedTime()
{
	return PerformanceCounterToSeconds(m_totalHPC);
}

uint64_t ProfilerReportEntry::GetChildrenTime(profile_measurement_t* node)
{
	uint64_t childrenTime = 0;
	profile_measurement_t* child = node->m_children;
	if (child != nullptr)
	{
		do 
		{
			childrenTime += child->GetElapsedTime();
			child = child->next;
		} 
		while (child != nullptr);
	}

	return childrenTime;
}
