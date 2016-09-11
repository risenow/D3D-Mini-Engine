#include "stdafx.h"
#include "Log.h"

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

void RuntimeLog::WriteLine(const std::string& ln)
{
    m_OutputFile << ln.c_str() << '\n';
}
void RuntimeLog::WriteLine(const std::wstring& ln)
{
    m_OutputFile << ln.c_str() << '\n';
}