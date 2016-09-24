#include "stdafx.h"
#include "CommandLineArgs.h"
#include "Log.h"
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
    std::string commandLine = ini.GetValue("CommandLine", "CommandLine", "");
    commandLine.erase(std::remove(commandLine.begin(), commandLine.end(), ' '), commandLine.end()); //erasing spaces

    ParseArgsFromString(commandLine);
    WriteToConsole();
}

void CommandLineArgs::WriteToConsole()
{
    for (std::string arg : m_Args)
    {
        LOG_CONS(arg + " ");
    }
    LOG_CONS(std::string("\n"));
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
        tempArgStr += argsStr[i];

        if ((i + 1)==argsStr.size() || argsStr[i + 1] == '/')
        {
            m_Args.push_back(tempArgStr);
            tempArgStr = "";
        }
    }
}