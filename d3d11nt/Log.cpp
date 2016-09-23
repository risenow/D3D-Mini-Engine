#include "stdafx.h"
#include "Log.h"
#include <iostream>

RuntimeLog& RuntimeLog::GetInstance()
{
    static RuntimeLog s_RuntimeLog;
    return s_RuntimeLog;
}

RuntimeLog::RuntimeLog()
{
    m_OutputFile = std::ofstream(LOG_FILE_OUTPUT_NAME, std::ofstream::out | std::ofstream::trunc);
}
RuntimeLog::~RuntimeLog()
{
    m_OutputFile.close();
}

void RuntimeLog::SetMode(RuntimeLogMode mode)
{
    m_Mode = mode;
}
