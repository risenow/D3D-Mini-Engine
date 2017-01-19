#pragma once

#include "System/stlcontainersintegration.h"

static const unsigned int DEFAULT_FRAME_TIMES_NUM_BUFFERED = 20;

class FrameMeasurer
{
public:
    FrameMeasurer(unsigned int frameTimesNumBuffered = DEFAULT_FRAME_TIMES_NUM_BUFFERED);

    void BeginMeasurement();
    void EndMeasurement();

    unsigned int GetFPSBasedOnPerSecondFrameCounter() const;
    float GetFPSBasedOnAverageFrameTime() const;
    float GetAverageFrameTime() const;

    void SetFrameTimesNumBuffered(unsigned int frameTimesNumBuffered);

    bool IsPerSecondFramesCountValueIsReady() const;
private:
    void AddFrameTime(unsigned int frameTime);

    void ResetPerSecondFrameCounter();
    void ResetAverageFrameTimeSolver();

    bool m_IsPerSecondFramesCountValuedIsReady;
    unsigned int m_PerSecondFramesCount;
    unsigned int m_BufferedPerSecondFramesCount;
    unsigned int m_FrameStartTime;
    unsigned int m_SecondMeasurementTime;

    unsigned int m_FrameTimesAllreadyBufferedNum;
    STLVector<unsigned int> m_BufferedFrameTimes; //use to calculate average frame time
};