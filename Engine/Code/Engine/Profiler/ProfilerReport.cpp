#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Profiler/ProfilerView.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include <algorithm>

static ProfilerReport* g_profilerReport = nullptr;

ProfilerReport::~ProfilerReport()
{

}

ProfilerReport::ProfilerReport()
{
	CommandRegister("profiler_report", ProfilerReport::ProfilerReportToConsole, "Prints report of previous frame to console. Options: tree | flat");
}

void ProfilerReport::GenerateReportTreeFromFrame(profile_measurement_t* root)
{
	if (m_root != nullptr)
		DestroyReportEntryRecursive(m_root);

	m_root = new ProfilerReportEntry(root->m_id); 
	m_frameRoot = m_root;
	m_root->PopulateTree(root); 
	m_root->Finalize(m_root, m_root->m_totalHPC);

	ProfilerReportEntry* fakeRoot = new ProfilerReportEntry("root");
	fakeRoot->m_children["root"] = m_root;
	m_root = fakeRoot;
	m_root->m_parent = fakeRoot;
	SortBySelfTime(m_root);
}

void ProfilerReport::GenerateReportFlatFromFrame(profile_measurement_t* root)
{
	if (m_root != nullptr)
		DestroyReportEntryRecursive(m_root);

	m_root = new ProfilerReportEntry(root->m_id); 
	m_frameRoot = m_root;

	ProfilerReportEntry* realRoot = m_root;
	ProfilerReportEntry* fakeRoot = new ProfilerReportEntry("root");
	fakeRoot->m_children["frame"] = m_root;
	realRoot->m_parent = fakeRoot;
	m_root = fakeRoot;

	profile_measurement_t* fakeMeasureRoot = new profile_measurement_t();
	fakeMeasureRoot->AddChild(root);

	m_root->PopulateFlat(fakeMeasureRoot); 
	m_root->Finalize(m_root, root->GetElapsedTime());

	SortBySelfTime(m_root);

	delete fakeMeasureRoot;
}

void ProfilerReport::SortBySelfTime(ProfilerReportEntry* entry)
{
	//copy over actual children
	entry->m_sortedChildren.clear();
	std::copy(entry->m_children.begin(), entry->m_children.end(), 
		std::back_inserter<std::vector<entryPair>>(entry->m_sortedChildren));

	std::sort(entry->m_sortedChildren.begin(), entry->m_sortedChildren.end(),
		[](const entryPair& l, const entryPair& r) {
		return l.second->m_selfHPC > r.second->m_selfHPC;
	});

	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = entry->m_children.begin(); it != entry->m_children.end(); it++)
	{
		SortByTotalTime(it->second);
	}
}

void ProfilerReport::SortByTotalTime(ProfilerReportEntry* entry)
{
	//copy over actual children
	entry->m_sortedChildren.clear();
	std::copy(entry->m_children.begin(), entry->m_children.end(), 
		std::back_inserter<std::vector<entryPair>>(entry->m_sortedChildren));

	std::sort(entry->m_sortedChildren.begin(), entry->m_sortedChildren.end(),
		[](const entryPair& l, const entryPair& r) {
			return l.second->m_totalHPC > r.second->m_totalHPC;
		});

	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = entry->m_children.begin(); it != entry->m_children.end(); it++)
	{
		SortByTotalTime(it->second);
	}
}

double ProfilerReport::GetTotalFrameTime()
{
	return m_frameRoot->GetTotalElapsedTime(); 
}

void ProfilerReport::DestroyReportEntryRecursive(ProfilerReportEntry* entry)
{
	std::map<std::string, ProfilerReportEntry*>::iterator it;
	for (it = entry->m_children.begin(); it != entry->m_children.end(); it++)
	{
		DestroyReportEntryRecursive(it->second);
	}

	delete entry;
}

ProfilerReport* ProfilerReport::CreateInstance()
{
	if (g_profilerReport == nullptr) 
	{
		g_profilerReport = new ProfilerReport();
	}
	return g_profilerReport; 
}

ProfilerReport* ProfilerReport::GetInstance()
{
	return g_profilerReport;
}

void ProfilerReport::ProfilerReportToConsole(Command& cmd)
{
	if (cmd.GetNextString().compare("flat") == 0)
	{
		g_profilerReport->GenerateReportFlatFromFrame(Profiler::GetInstance()->ProfileGetPreviousFrame());
		ConsolePrintf(ProfilerView::GetInstance()->ProfileEntryToStringIndented(g_profilerReport->m_root).c_str());
	}
	else
	{
		//defaults to tree view
		g_profilerReport->GenerateReportTreeFromFrame(Profiler::GetInstance()->ProfileGetPreviousFrame());
		ConsolePrintf(ProfilerView::GetInstance()->ProfileEntryToStringIndented(g_profilerReport->m_root).c_str());
	}
}
