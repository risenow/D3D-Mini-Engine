#pragma once
#include <Windows.h>
#include <string>
#include "System/stlcontainersintegration.h"

class FileChangesMonitor
{
public:
	enum Flags
	{
		Flag_NoRepeatingFileNames = 1u
	};
	typedef unsigned long FlagsType;

	FileChangesMonitor();
	FileChangesMonitor(const std::wstring& dirPath);

	bool GatherChanges(STLVector<std::wstring>& changedFiles, FlagsType flags);

	std::wstring GetDirPath() const;
private:
	HANDLE m_DirHandle;
	std::wstring m_DirPath;
};