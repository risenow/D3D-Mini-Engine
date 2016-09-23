#pragma once
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>

constexpr char* LOG_FILE_OUTPUT_NAME = "log.txt";

enum RuntimeLogModeMasks
{
    RuntimeLogMode_FILE_OUTPUT = 1u,
    RuntimeLogMode_CONSOLE_OUTPUT = 1u << 1
};
typedef unsigned int RuntimeLogMode;

class RuntimeLog
{
public:
    static RuntimeLog& GetInstance();

    template<class T>
    void WriteLineUsingMode(const T& ln, RuntimeLogMode mode)
    {
        if (mode & RuntimeLogMode_FILE_OUTPUT)
            m_OutputFile << ln.c_str() << '\n';
        if (mode & RuntimeLogMode_CONSOLE_OUTPUT)
            std::cout << ln.c_str() << '\n';
    }

    template<class T>
    void WriteLine(const T& ln)
    {
        WriteLineUsingMode(ln, m_Mode);
    }
    
    void SetMode(RuntimeLogMode mode);
private:
    RuntimeLog();
    ~RuntimeLog();

    RuntimeLogMode m_Mode;
    std::ofstream m_OutputFile;
};

#define LOG(str) (RuntimeLog::GetInstance().WriteLine(str))
#define LOG_CONS(str) (RuntimeLog::GetInstance().WriteLineUsingMode(str, RuntimeLogMode_CONSOLE_OUTPUT))
#define LOG_FILE(str) (RuntimeLog::GetInstance().WriteLineUsingMode(str, RuntimeLogMode_FILE_OUTPUT))
