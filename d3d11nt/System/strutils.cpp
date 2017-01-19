#include "stdafx.h"
#include "System/strutils.h"

std::wstring strtowstr(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wstr = converter.from_bytes(str);
    return wstr;
}
std::string wstrtostr(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string str = converter.to_bytes(wstr);
    return str;
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

template<>
void ToString(const std::wstring& in, std::string& out)
{
    out = wstrtostr(in);
}
template<>
void ToString(const std::string& in, std::string& out)
{
    out = in;
}

template<>
void ToString(const std::string& in, std::wstring& out)
{
    out = strtowstr(in);
}
template<>
void ToString(const std::wstring& in, std::wstring& out)
{
    out = in;
}

template<>
std::wstring GetWideString(const wcptr& str)
{
    return std::wstring(str);
}
template<>
std::wstring GetWideString(const std::string& str)
{
    return strtowstr(str);
}
template<>
std::wstring GetWideString(const cptr& str)
{
    return strtowstr(std::string(str));
}


template<>
cptr GetStringContent(const cptr& str)
{
    return (cptr)str;
}

template<>
void WriteToConsole(const std::wstring& str)
{
    std::wcout << str;
}

template<>
bool IsSpace(wchar_t c) // T== wchar
{
    return c == L' ';
}

