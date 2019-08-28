#include "stdafx.h"
#include "System/CommandLineArgs.h"
#include "System/Log.h"
#include <algorithm>
#include <iostream>

CommandLineArgs::CommandLineArgs() {}
CommandLineArgs::CommandLineArgs(char* args[], size_t size)
{
    for (unsigned int i = 0; i < size; i++)
    {
        m_Args.push_back(std::string(args[i]));
    }
    WriteToConsole();
}

CommandLineArgs::CommandLineArgs(const IniFile& ini)
{
    std::string commandLine = ini.GetStrValue("CommandLine", "CommandLine", "");
    commandLine.erase(std::remove(commandLine.begin(), commandLine.end(), ' '), commandLine.end()); //erasing spaces

    ParseArgsFromString(commandLine);
    WriteToConsole();
}

void CommandLineArgs::WriteToConsole()
{
    std::wstring logBuffer;
	StringRuntimeLogStream logBufferStream(&logBuffer);
    LOG_BUFFER(std::string("Commandline Args:\n"), logBuffer);
    for (std::string& arg : m_Args)
    {
		popGetLogger().WriteUsingMode({ &logBufferStream }, std::string(arg + " "), 0);
    }
	popGetLogger().WriteUsingMode({ &LOG_CONSOLE_STREAM() }, logBuffer, 0);
}

bool CommandLineArgs::HasArg(const std::string& param) const
{
    for (unsigned int i = 0; i < m_Args.size(); i++)
    {
        if (m_Args[i] == param)
            return true;
    }
    return false;
}

void CommandLineArgs::ParseArgsFromString(const std::string& argsStr)
{
    std::string tempArgStr;
    for (unsigned int i = 0; i < argsStr.size(); i++)
    {
        if (argsStr[i] != '/')
            tempArgStr += argsStr[i];

        if ((i + 1)==argsStr.size() || argsStr[i + 1] == '/')
        {
            m_Args.push_back(tempArgStr);
            tempArgStr = "";
        }
    }
}