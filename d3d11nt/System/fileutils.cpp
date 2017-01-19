#include "stdafx.h"
#include "System/fileutils.h"
#include "System/strutils.h"
#include <Windows.h>

bool FileExists(const std::string& fileName)
{
    DWORD dwAttrib = GetFileAttributes(strtowstr(fileName).c_str());

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
           !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void CreateSimpleFile(const std::string& fileName)
{
    HANDLE handle = CreateFile(strtowstr(fileName).c_str(), GENERIC_READ | GENERIC_WRITE, 
                               0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
}

void CreateFileIfNotExists(const std::string& fileName)
{
    if (!FileExists(fileName))
        CreateSimpleFile(fileName);
}