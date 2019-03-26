#include "stdafx.h"
#include <thread>
#include "System/timeutils.h"
#include "Graphics/FrameRateLock.h"

FrameRateLock::FrameRateLock(unsigned long targetFPS)
{
	m_TargetMilisecondsPerFrame = 1.0 / ((double)targetFPS / 1000.0);
}

void FrameRateLock::OnFrameBegin()
{
	m_FrameStartTime = GetPerfomanceCounterMicroseconds() / 1000.0;
}

void FrameRateLock::OnFrameEnd()
{
	FrameTimeType frameTime = GetPerfomanceCounterMicroseconds() / 1000.0 - m_FrameStartTime;
	FrameTimeType timeDiff = m_TargetMilisecondsPerFrame - frameTime;
	if (timeDiff > 0)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(int(timeDiff)));
	}
}
