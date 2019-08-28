#include "stdafx.h"
#include "System/FileChangesMonitor.h"
#include "System/typesalias.h"

FileChangesMonitor::FileChangesMonitor() : m_DirHandle(NULL)
{
	
}
FileChangesMonitor::FileChangesMonitor(const std::wstring& dirPath) : m_DirPath(dirPath)
{
	m_DirHandle = CreateFile(
		m_DirPath.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
}
std::wstring FileChangesMonitor::GetDirPath() const
{
	return m_DirPath;
}
bool FileChangesMonitor::GatherChanges(STLVector<std::wstring>& changedFiles, FlagsType flags)
{
	FILE_NOTIFY_INFORMATION strFileNotifyInfo[1024];
	DWORD dwBytesReturned = 0;

	byte_t* prevFileNotifyEntryPtr = nullptr;
	byte_t* currentFileNotifyEntryPtr = (byte_t*)&strFileNotifyInfo[0];
	bool success = (ReadDirectoryChangesW(m_DirHandle, (LPVOID)&strFileNotifyInfo, sizeof(strFileNotifyInfo), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &dwBytesReturned, NULL, NULL) != 0);
	do
	{
		FILE_NOTIFY_INFORMATION& currentFileNotifyEntry = (*(FILE_NOTIFY_INFORMATION*)currentFileNotifyEntryPtr);
		currentFileNotifyEntry.FileName[currentFileNotifyEntry.FileNameLength / sizeof(wchar_t)] = L'\0';
		changedFiles.push_back(currentFileNotifyEntry.FileName);
		prevFileNotifyEntryPtr = currentFileNotifyEntryPtr;
		currentFileNotifyEntryPtr += currentFileNotifyEntry.NextEntryOffset;
	} while (currentFileNotifyEntryPtr != prevFileNotifyEntryPtr);

	if (flags & Flag_NoRepeatingFileNames)
	{
		for (unsigned long i = 0; i < changedFiles.size(); i++)
		{
			for (unsigned long j = 0; j < changedFiles.size(); j++)
			{
				if (j != i && changedFiles[j] == changedFiles[i])
					changedFiles.erase(changedFiles.begin() + j);
			}
		}
	}

	return success;
}