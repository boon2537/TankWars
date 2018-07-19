//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include "Engine/Core/StringUtils.hpp"
#include <Windows.h>
#include <time.h>

class LocalTimeData
{
public:
	LocalTimeData()
	{
		::QueryPerformanceFrequency((LARGE_INTEGER*) &m_HPC_PerSeconds); 
		m_secondPerHPC = 1.0f / (double) m_HPC_PerSeconds;
	}

public:
	uint64_t m_HPC_PerSeconds;
	double m_secondPerHPC;
};

static LocalTimeData gTimeSystem;

//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}

std::string GetFormatedDateTime()
{
	//Time reference: https://msdn.microsoft.com/en-us/library/a442x3ye.aspx
	struct tm newtime;   
	__time64_t long_time;  
	char buf[80]; 

	_time64( &long_time );
	_localtime64_s( &newtime, &long_time );

	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &newtime);

	return buf;
}

uint64_t GetPerformanceCounter()
{
	uint64_t hpc;
	::QueryPerformanceCounter((LARGE_INTEGER*)&hpc); 
	return hpc; 
}

double PerformanceCounterToSeconds(const uint64_t& hpc)
{
	return (double) hpc * gTimeSystem.m_secondPerHPC;
}

uint64_t ConvertSecondsToPerformanceCounter(float seconds)
{
	return static_cast<uint64_t>(seconds / gTimeSystem.m_secondPerHPC);
}

std::string TimePerfCountToString(const uint64_t& hpc)
{
	double seconds = PerformanceCounterToSeconds(hpc);

	if (seconds < 0.1)
	{		
		if (seconds * 1000.0 < 0.1)
		{
			//use microseconds
			return Stringf("%.3f us", seconds * 1000000.0);
		}
		else
		{
			//use ms
			return Stringf("%.3f ms", seconds * 1000.0);
		}
	}	
	else
	{
		//use seconds
		return Stringf("%.3f s", seconds);
	}
}
