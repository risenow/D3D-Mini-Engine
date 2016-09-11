#pragma once
#include <fstream>
#include <cstring>

constexpr char* LOG_FILE_OUTPUT_NAME = "log.txt";

class RuntimeLog
{
public:
    static RuntimeLog& GetInstance();

    void WriteLine(const std::string& ln);
    void WriteLine(const std::wstring& ln);
private:
    RuntimeLog();
    ~RuntimeLog();

    std::ofstream m_OutputFile;
};

#define LOG(str) (RuntimeLog::GetInstance().WriteLine(str))
