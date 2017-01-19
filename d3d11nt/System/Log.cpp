#include "stdafx.h"
#include "System/Log.h"
#include "System/timeutils.h"
#include <iostream>

RuntimeLog& RuntimeLog::GetInstance()
{
    static RuntimeLog s_RuntimeLog;
    return s_RuntimeLog;
}

RuntimeLog::RuntimeLog()
{
    m_OutputFile = std::wofstream(LOG_FILE_OUTPUT_NAME, std::ofstream::out | std::ofstream::trunc);
}
RuntimeLog::~RuntimeLog()
{
    m_OutputFile.close();
}
void RuntimeLog::SetMode(RuntimeLogMode mode)
{
    m_Mode = mode;
}
void RuntimeLog::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}
bool RuntimeLog::GetEnabled()
{
    return m_Enabled;
}
