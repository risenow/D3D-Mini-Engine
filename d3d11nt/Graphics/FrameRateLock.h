#pragma once

class FrameRateLock
{
public:
	typedef double FrameTimeType;

	FrameRateLock(unsigned long targetFPS);

	void OnFrameBegin();
	void OnFrameEnd();
private:
	FrameTimeType m_FrameStartTime;
	FrameTimeType m_TargetMilisecondsPerFrame;
};