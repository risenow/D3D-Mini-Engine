#pragma once
#include "IniFile.h"
#include <vector>
#include "stlcontainersintegration.h"
#include <string>

const std::string COMMAND_LINE_ARG_DEBUG_D3D_DEVICE_ENABLE = "enableDebugDevice";
const std::string COMMAND_LINE_ARG_SINGLETHREADED_DEVICE_ENABLE = "enableSinglethreadedDevice";

class CommandLineArgs
{
public:
    CommandLineArgs();
    CommandLineArgs(char* args[], size_t size);
    CommandLineArgs(const IniFile& info);

    void WriteToConsole();

    bool HasArg(const std::string& param) const;
private:
    std::vector<std::string> m_Args;
    std::string m_FileName;
};