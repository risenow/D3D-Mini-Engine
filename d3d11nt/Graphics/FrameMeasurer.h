#pragma once

#include "System/stlcontainersintegration.h"

static const unsigned int DEFAULT_FRAME_TIMES_NUM_BUFFERED = 10;

class FrameMeasurer
{
public:
	typedef double FrameTimeType;
	
    FrameMeasurer(unsigned int frameTimesNumBuffered = DEFAULT_FRAME_TIMES_NUM_BUFFERED);

    void BeginMeasurement();
    void EndMeasurement();

    unsigned int GetFPSBasedOnPerSecondFrameCounter() const;
    float GetFPSBasedOnAverageFrameTime() const;
	FrameTimeType GetAverageFrameTime() const;

    void SetFrameTimesNumBuffered(unsigned int frameTimesNumBuffered);

    bool IsPerSecondFramesCountValueIsReady() const;
private:
    void AddFrameTime(FrameTimeType frameTime);

    void ResetPerSecondFrameCounter();
    void ResetAverageFrameTimeSolver();

    bool m_IsPerSecondFramesCountValuedIsReady;
    unsigned int m_PerSecondFramesCount;
    unsigned int m_BufferedPerSecondFramesCount;
	FrameTimeType m_FrameStartTime;
	FrameTimeType m_SecondMeasurementTime;

    unsigned int m_FrameTimesAllreadyBufferedNum;
    STLVector<FrameTimeType> m_BufferedFrameTimes; //used to calculate average frame time
};