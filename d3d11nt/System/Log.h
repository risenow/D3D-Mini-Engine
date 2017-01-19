#pragma once
#include "System/strutils.h"
#include "System/timeutils.h"
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>

constexpr char* LOG_FILE_OUTPUT_NAME = "log.txt";

enum RuntimeLogModeMasks
{
    RuntimeLogMode_FileOutput = 1u,
    RuntimeLogMode_ConsoleOutput = 1u << 1,
    RuntimeLogMode_WriteTime = 1u << 2,
    RuntimeLogMode_WriteLine = 1u << 3
};
typedef unsigned int RuntimeLogMode;

class RuntimeLog
{
public:
    static RuntimeLog& GetInstance();

    template<class T>
    void WriteUsingMode(const T& str, RuntimeLogMode mode)
    {
        if (!m_Enabled)
            return;

        std::wstring compiledStr = GetWideString(str);
        CompileStr(compiledStr, mode);
        if (mode & RuntimeLogMode_FileOutput)
            m_OutputFile << GetWideString(compiledStr);
        if (mode & RuntimeLogMode_ConsoleOutput)
        {
            WriteToConsole(compiledStr);
        }
    }

    template<class T>
    void Write(const T& str)
    {
        WriteUsingMode(str, m_Mode);
    }

    template<class T>
    void WriteToBufferUsingMode(const T& str, T& destBuffer, RuntimeLogMode mode)
    {
        T compiledStr = str;
        CompileStr(compiledStr, mode);
        destBuffer += compiledStr;
    }
    
    void SetMode(RuntimeLogMode mode);
    void SetEnabled(bool enabled);

    bool GetEnabled();
private:
    RuntimeLog();
    ~RuntimeLog();

    template<class T>
    void CompileStr(T& str, RuntimeLogMode mode) // T == string/wstring
    {
        if (mode & RuntimeLogMode_WriteTime)
        {
            T timeValueString;
            ToString(GetMilisecondsSinceProgramStart(), timeValueString);
            T timeSectionBegin;
            ToString(std::string("[TIME:"), timeSectionBegin);
            T timeSectionEnd;
            ToString(std::string("] "), timeSectionEnd);
            str = timeSectionBegin + timeValueString + timeSectionEnd + str;
        }
        if (mode & RuntimeLogMode_WriteLine)
        {
            T endlStr;
            ToString(std::string("\n"), endlStr);
            str = str + endlStr;
        }
    }
    bool m_Enabled;
    RuntimeLogMode m_Mode;
    std::wofstream m_OutputFile;
};

#define popGetLogger() (RuntimeLog::GetInstance())
#define LOG_BUFFER(str, dest) (RuntimeLog::GetInstance().WriteToBufferUsingMode(str, dest, RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine))
#define LOG(str) (RuntimeLog::GetInstance().Write(str))
#define LOG_CONS(str) (RuntimeLog::GetInstance().WriteUsingMode(str, RuntimeLogMode_ConsoleOutput | RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine))
#define LOG_FILE(str) (RuntimeLog::GetInstance().WriteUsingMode(str, RuntimeLogMode_FileOutput | RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine))
