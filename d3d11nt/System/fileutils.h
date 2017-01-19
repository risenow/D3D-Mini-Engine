#pragma once
#include <string>

bool FileExists(const std::string& fileName);
void CreateSimpleFile(const std::string& fileName);

void CreateFileIfNotExists(const std::string& fileName);