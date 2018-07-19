#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

class AnalogJoystick
{
public:
	~AnalogJoystick() {}
	AnalogJoystick() {}

	void ApplyCorrection()
	{
		float rawR = sqrtf((m_rawPosition.x * m_rawPosition.x) + (m_rawPosition.y * m_rawPosition.y));
		m_angleDegrees = Atan2Degress(m_rawPosition.y, m_rawPosition.x);
		m_magnitude = RangeMapFloat(rawR / 32767.f, m_innerDeadZone, m_outerDeadZone, 0, 1);
		m_magnitude = ClampFloatZeroToOne(m_magnitude);

		m_correctedPosition.x = m_magnitude * CosDegrees(m_angleDegrees);
		m_correctedPosition.y = m_magnitude * SinDegrees(m_angleDegrees);
	}

	void Reset()
	{
		m_angleDegrees = 0;
		m_magnitude = 0;
		m_correctedPosition = Vector2(0, 0);
		m_rawPosition = Vector2(0, 0);
	}

	Vector2 m_rawPosition;
	Vector2 m_correctedPosition;
	float m_innerDeadZone;
	float m_outerDeadZone;
	float m_magnitude;
	float m_angleDegrees;
};