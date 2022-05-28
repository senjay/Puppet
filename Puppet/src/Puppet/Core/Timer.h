#pragma once

#include <chrono>

namespace Puppet {

	class Timer
	{
	public:
		Timer()
		{
			m_Start=std::chrono::high_resolution_clock::now();
		}
		//reset and return Elapsed Seconds
		float Timer::Reset()
		{
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
			float Mill = (float)std::chrono::duration_cast<std::chrono::milliseconds>(end - m_Start).count();
			m_Start = end;
			return Mill*0.001f;
		}
		float Timer::Elapsed()
		{
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
			return (float)std::chrono::duration_cast<std::chrono::milliseconds>(end- m_Start).count();
		}
		float Timer::ElapsedSeconds()
		{
			return Elapsed()*0.001f;
		}
		float Timer::ElapsedMillis()
		{
			return Elapsed();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

}
