#pragma once
namespace Puppet {
	class TimeStep
	{
	public:
		TimeStep(float time=0.0f) :m_Time(time)
		{

		}
		float GetSeconds()const { return m_Time; }
		float GetMillseconds()const { return m_Time*1000.0f; }
		//return Seconds
		operator float() const { return m_Time; }
	private:
		float m_Time;
	};

}