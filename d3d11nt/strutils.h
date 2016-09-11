#pragma once
#include <string>
#include <cstring>
#include <codecvt>

std::wstring strtowstr(const std::string & str);
bool StrIsRealNumber(const std::string& str);
bool StrIsInt(const std::string& str);
bool StrIsUnsignedInt(const std::string& str);