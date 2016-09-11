#include "stdafx.h"
#include "CommandLineArgs.h"
#include <iostream>

CommandLineArgs::CommandLineArgs() {}
CommandLineArgs::CommandLineArgs(char* args[], size_t size)
{
    for (int i = 0; i < size; i++)
    {
        m_Args.push_back(std::string(args[i]));
    }
    WriteToConsole();
}

void CommandLineArgs::WriteToConsole()
{
    for (std::string arg : m_Args)
    {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
}

bool CommandLineArgs::HasArg(const std::string& param) const
{
    for (int i = 0; i < m_Args.size(); i++)
    {
        if (m_Args[i] == param)
            return true;
    }
    return false;
}