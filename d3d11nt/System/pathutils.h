#pragma once
#include <string>
#include "System/typesalias.h"

bool IsPathToFile(const std::wstring& path);
std::wstring ReducePathRank(const std::wstring& path, unsigned long rank);
std::wstring ReducePathToRankFromStart(const std::wstring& path, unsigned long rank);

unsigned long GetPathRank(const std::wstring& path);

void NormalizePath(std::wstring& path);
void NormalizePath(wchar_t* path);