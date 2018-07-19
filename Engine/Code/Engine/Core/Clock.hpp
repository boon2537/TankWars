#pragma once

#include "Engine/Core/Time.hpp"
#include <stdint.h>
#include <vector>

struct time_unit_t
{
	time_unit_t()
	{
		ms = 0;
		seconds = 0;
		hpc = 0;
	}

	unsigned int ms;
	double seconds;
	uint64_t hpc;
};

class Clock
{
public:
	Clock(Clock* parent = nullptr);
	Clock(const Clock& c) = delete; //enforce NO copy constructor

	void BeginFrame();
	void Reset();
	void Advance(uint64_t elapsed);

	void AddChild(Clock* child);

	// returns time since this clock 
	// using the current hpc, and our start hpc
	// to get the most up-to-date time.
	double GetClockCurrentTime() const; 

	void SetPaused(bool paused); 
	void SetScale(float scale);

public:
	time_unit_t m_frame;
	time_unit_t m_total;

private:
	double m_scale;
	bool m_paused;

	uint64_t m_lastFrameHPC;
	unsigned int m_frameCount;

	Clock* m_parent;
	std::vector<Clock*> m_children;
};

//------------------------------------------------------------------------
// C Functions
//------------------------------------------------------------------------
// could calso be static Clock* Clock::GetMaster(); 
Clock* GetMasterClock(); 

void ClockSystemStartup();

// convenience - calls begin frame on the master clock
void ClockSystemBeginFrame();

// I now move this here - as this now refers to the master clock
// who is keeping track of the starting reference point. 
double GetSystemCurrentTime(); 

// Also add convenience functions so I'm not constantly fetching the master clock, like
double GetSystemDeltaTime();

/*
void UseCase()
{
	Clock* clock = GetMasterClock();
	float total_time = clock->m_total;
}
*/