#include "stdafx.h"
#include "Graphics/FrameMeasurer.h"
#include "System/timeutils.h"

FrameMeasurer::FrameMeasurer(unsigned int frameTimesNumBuffered) : m_FrameStartTime(0), m_PerSecondFramesCount(0),
                                                                   m_SecondMeasurementTime(0), m_IsPerSecondFramesCountValuedIsReady(false),
                                                                   m_BufferedPerSecondFramesCount(0), m_FrameTimesAllreadyBufferedNum(0)
{
    SetFrameTimesNumBuffered(frameTimesNumBuffered);
}

bool FrameMeasurer::IsPerSecondFramesCountValueIsReady() const
{
    return m_IsPerSecondFramesCountValuedIsReady;
}

void FrameMeasurer::ResetPerSecondFrameCounter()
{
    m_SecondMeasurementTime = 0;
    m_PerSecondFramesCount = 0;
    m_BufferedPerSecondFramesCount = 0;
    m_IsPerSecondFramesCountValuedIsReady = false;
}

void FrameMeasurer::ResetAverageFrameTimeSolver()
{
    for (int i = 0; i < m_BufferedFrameTimes.size(); i++)
    {
        m_BufferedFrameTimes[i] = 0;
    }
    m_FrameTimesAllreadyBufferedNum = 0;
}

void FrameMeasurer::SetFrameTimesNumBuffered(unsigned int frameTimesNumBuffered)
{
    m_BufferedFrameTimes.resize(frameTimesNumBuffered);
    ResetAverageFrameTimeSolver();
}

void FrameMeasurer::AddFrameTime(unsigned int frameTime)
{
    if (m_FrameTimesAllreadyBufferedNum < m_BufferedFrameTimes.size())
        m_FrameTimesAllreadyBufferedNum++;

    if (m_BufferedFrameTimes.size() >= 2)
    {
        for (int i = 1; i < m_BufferedFrameTimes.size(); i++)
        {
            m_BufferedFrameTimes[i - 1] = m_BufferedFrameTimes[i];
        }
    }
    m_BufferedFrameTimes[m_BufferedFrameTimes.size() - 1] = frameTime;
}

void FrameMeasurer::BeginMeasurement()
{
    m_FrameStartTime = GetMilisecondsSinceProgramStart();
}

void FrameMeasurer::EndMeasurement()
{
    unsigned int frameTime = GetMilisecondsSinceProgramStart() - m_FrameStartTime;
    AddFrameTime(frameTime);
    m_SecondMeasurementTime += frameTime;
    m_BufferedPerSecondFramesCount++;

    static const unsigned int milisecPerSec = 1000;
    if (m_SecondMeasurementTime >= milisecPerSec)
    {
        m_SecondMeasurementTime = m_SecondMeasurementTime - milisecPerSec;  // for condition when m_SecondMeasurementTime > milisecPerSec
        m_PerSecondFramesCount = m_BufferedPerSecondFramesCount;
        m_BufferedPerSecondFramesCount = 0;
        m_IsPerSecondFramesCountValuedIsReady = true;
    }
}

unsigned int FrameMeasurer::GetFPSBasedOnPerSecondFrameCounter() const
{
    return m_IsPerSecondFramesCountValuedIsReady ? m_PerSecondFramesCount : m_BufferedPerSecondFramesCount;
}

float FrameMeasurer::GetAverageFrameTime() const
{
    unsigned int frameTimesSumm = 0.0f;

    for (int i = 0; i < m_BufferedFrameTimes.size(); i++)
    {
        frameTimesSumm += m_BufferedFrameTimes[i];
    }

    return (float)frameTimesSumm / (float)m_FrameTimesAllreadyBufferedNum;
}

float FrameMeasurer::GetFPSBasedOnAverageFrameTime() const
{
    static const unsigned int milisecPerSec = 1000;
    return (float)milisecPerSec/GetAverageFrameTime();
}