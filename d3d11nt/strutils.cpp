#include "stdafx.h"
#include "strutils.h"

std::wstring strtowstr(const std::string & str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wstr = converter.from_bytes(str);
    return wstr;
}

bool StrIsRealNumber(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("-0123456789.") == std::string::npos);
}
bool StrIsInt(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("0123456789") == std::string::npos);
}
bool StrIsUnsignedInt(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("0123456789.") == std::string::npos);
}