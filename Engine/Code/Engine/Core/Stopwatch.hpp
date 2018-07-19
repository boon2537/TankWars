#pragma once

#include "Engine/Core/Clock.hpp"

//------------------------------------------------------------------------
// StopWatch - meant for game systems
// as it will use frame-start time for calculating
//------------------------------------------------------------------------
class StopWatch
{
public:
	// should default to Master if ref_clock is set to nullptr
	StopWatch(Clock* refClock = nullptr); 

	// change our reference clock. 
	// should maintain elapsed time 
	void SetClock(Clock* refClock); 

	// sets the "interval time" for
	// this stopWatch.
	// takes seconds to be easy to use
	// but under the hood it will use hpc
	void SetTimer(float seconds); 

	// if we really just want to use it as a timer
	// this returns how long since the last reset
	float GetElapsedTime();
	uint64_t GetElapsedHPC();

	// elapsed time divided by the interval
	float GetNormalizedElapsedTime(); 

	// returns true if the interval has elapsed;
	bool HasElapsed(); 

	// resets the interval
	// (get_elapsed_time() should return 0.0f);
	void Reset(); 

	// checks the interval, if it has elapsed, will
	// return true and reset.  If not, will just return false (no reset)
	bool CheckAndReset(); 

	// returns has_elapsed(), and if so
	// will subtract one interval from 
	// the elapsed time (but maintaining 
	// any extra time we have accrued)
	bool Decrement(); 

	// like decrement, but returns how many 
	// times we have elapsed our timer
	// so say our timer was 0.5 seconds, 
	// and get_elapsed_time() was 2.2s.  
	// decrement_all() would return 4, and 
	// update itself so that get_elapsed_time()
	// would return 0.2s;
	unsigned int DecrementAll(); 

	void Pause();
	void Resume();

public:
	// clock to use as reference
	Clock* m_reference = nullptr; 

	// reference times; 
	// notice using these member
	// variables, our stopwatch doesn't need
	// an "update", like it would if we
	// were accumulating deltas
	uint64_t m_start_HPC; 
	uint64_t m_intervalHPC; 

private:
	uint64_t m_cachedElapsedHPC;
	uint64_t m_deductHPC; //used to make decrement and pause work

	bool m_isPaused = false;
	uint64_t m_pauseRefHPC;
};
