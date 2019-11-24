#include "stdafx.h"
#include "System/IniFile.h"
#include "System/logicsafety.h"
#include "System/fileutils.h"
#include <algorithm>
#include <iostream>
#include <fstream>


IniFile::IniFile() {}
IniFile::IniFile(const std::string& fileName, unsigned int loadingFlags)
{
    Load(fileName, loadingFlags);
}
void IniFile::Load(const std::string& fileName, unsigned int loadingFlags)
{
    if (loadingFlags & LoadingFlags_FileMayNotExist)
        CreateFileIfNotExists(fileName);

    m_CurrentSection = "";
    m_AtLeastOneSectionRead = false;

    std::ifstream iniFile(fileName);
    m_Loaded = iniFile.is_open();
    if (!m_Loaded)
        return;

    std::string s;
    bool eof = false;
    while (!eof)
    {
        eof = std::getline(iniFile, s).eof();
        if (!s.size())
            continue;
        if (s[0] == ';' || s[0] == '#')
            continue;
        EraseAllSpacesFromString(s);
        if (s[0] == '[')
        {
            m_CurrentSection = ReadSectionFromLine(s);
            m_AtLeastOneSectionRead = true;
            continue;
        }
        ReadPropertyFromLine(s);
    }

    iniFile.close();
}
void IniFile::Save(const std::string& fileName)
{
    std::ofstream file(fileName, std::ofstream::trunc | std::ofstream::out);
    for (IniStructure::iterator i = m_Ini.begin(); i != m_Ini.end(); i++)
    {
        file << '[' << i->first << "]\n"; //section
        for (IniProperties::iterator j = i->second.begin(); j != i->second.end(); j++)
        {
            file << j->first << '='; //name
            
            //values
            for (unsigned int k = 0; k < j->second.size(); k++)
            {
                std::string valStr = j->second[k] + ((k < j->second.size() - 1) ? "," : "\n");
                file << valStr;
            }

        }
    }
    file.close();
}
bool IniFile::IsLoaded() const
{
    return m_Loaded;
}
int IniFile::GetIntValue(popDeclareGetValParams, int defaultValue) const
{
    return std::stoi(GetStrValue(section, name, std::to_string(defaultValue)));
}
unsigned int IniFile::GetUIntValue(popDeclareGetValParams, unsigned int defaultValue) const
{
    int val = GetIntValue(section, name, (int)defaultValue);
    popAssert(val >= 0);
    if (val < 0)
        return defaultValue;
    return (unsigned int)val;
}
float IniFile::GetFloatValue(popDeclareGetValParams, float defaultValue) const
{
    return std::stof(GetStrValue(section, name, std::to_string(defaultValue)));
}

std::string IniFile::GetStrValue(popDeclareGetValParams, const std::string& defaultValue) const
{
    STLVector<std::string>* values = GetValues(section, name);
    if (!values || !(*values).size())
        return defaultValue;

    return (*values)[0];
}

std::string IniFile::GetValue(popDeclareGetValParams, const std::string& defaultValue) const
{
    return GetStrValue(section, name, defaultValue);
}
int IniFile::GetValue(popDeclareGetValParams, int defaultValue) const
{
    return GetIntValue(section, name, defaultValue);
}
unsigned int IniFile::GetValue(popDeclareGetValParams, unsigned int defaultValue) const
{
    return GetUIntValue(section, name, defaultValue);
}
float IniFile::GetValue(popDeclareGetValParams, float defaultValue) const
{
    return GetFloatValue(section, name, defaultValue);
}

//public
void IniFile::GetValues(popDeclareGetValParams, const STLVector<std::string>& defaultValues, STLVector<std::string>& outValues)
{
    popAssert(defaultValues.size());
    STLVector<std::string>* values = GetValues(section, name);
    if (!values || !(*values).size())
    {
        outValues = defaultValues;
        return;
    }

    outValues = *values;
}

//private method for internal use only
STLVector<std::string>* IniFile::GetValues(popDeclareGetValParams) const
{
    popAssert(m_Loaded);

    IniStructure::const_iterator sectionIter = m_Ini.find(section);
    if (sectionIter == m_Ini.end())
    {
        popAssert(false);
        return nullptr;
    }
    IniProperties::const_iterator propertyIter = sectionIter->second.find(name);
    if (propertyIter == sectionIter->second.end())
    {
        popAssert(false);
        return nullptr;
    }
    return (STLVector<std::string>*)&propertyIter->second;
}

bool IniFile::IsCommentStartChar(char c)
{
    return (c == ';' || c == '#');
}
std::string IniFile::ReadSectionFromLine(const std::string& ln)
{
    unsigned int charIndex = 0;
    popAssert(ln[0] == '[');

    while (charIndex < ln.size() && (!IsCommentStartChar(ln[charIndex])))
    {
        charIndex++;
    }
    popAssert(ln[charIndex - 1] == ']');

    unsigned int substrCharCount = charIndex - 2;
    return ln.substr(1, substrCharCount); //exclusive [ & ]  //-2 because first param is offset
}
void IniFile::ReadPropertyFromLine(const std::string& ln)
{
    STLVector<std::string> propertyValues;

    unsigned int equalSignCharIndex = ln.find('=', 0);
    popAssert(equalSignCharIndex != std::string::npos);
    popAssert(m_AtLeastOneSectionRead);

    std::string propertyName = ReadPropertyNameFromLine(ln, equalSignCharIndex);
    std::string propertyWholeStringValues = ReadPropertyValuesAsWholeStringFromLine(ln, equalSignCharIndex);
    DecomposeWholeStringPropertyValues(propertyWholeStringValues, propertyValues);

    m_Ini[m_CurrentSection][propertyName] = propertyValues;
}
std::string IniFile::ReadPropertyNameFromLine(const std::string& ln, unsigned int equalSignPos)
{
    popAssert(!IsCommentStartChar(ln[0]));
    
    unsigned int firstVariantCommentStartCharIndex = ln.find(';', 0);
    popAssert(firstVariantCommentStartCharIndex == std::string::npos || firstVariantCommentStartCharIndex < equalSignPos);

    return ln.substr(0, equalSignPos);
}
std::string IniFile::ReadPropertyValuesAsWholeStringFromLine(const std::string& ln, unsigned int equalSignPos)
{
    unsigned int charIndex = equalSignPos + 1;
    unsigned int charCount = 0;

    while (charIndex < ln.size() && (!IsCommentStartChar(ln[charIndex])))
    {
        charIndex++;
        charCount++;
    }

    return ln.substr(equalSignPos + 1, charCount);
}

void IniFile::DecomposeWholeStringPropertyValues(const std::string& wholeStringPropertyValues, STLVector<std::string>& values)
{
    unsigned int valuesNum = std::count(wholeStringPropertyValues.begin(), wholeStringPropertyValues.end(), ',') + 1;
    values.resize(valuesNum);

    std::string tempValueString;
    int j = 0;
    for (unsigned int i = 0; i < wholeStringPropertyValues.size(); i++)
    {
        if (wholeStringPropertyValues[i] == ',')
        {
            values[j] = tempValueString;
            tempValueString = "";
            j++;
			continue;
        }
        tempValueString += wholeStringPropertyValues[i];
    }
    values[j] = tempValueString;
}

AutoSaveIniFile::AutoSaveIniFile() : IniFile() {}
AutoSaveIniFile::AutoSaveIniFile(const std::string& fileName, unsigned int loadingFlags)
{
    IniFile::Load(fileName, loadingFlags);
    m_LoadedFileName = fileName;
}
AutoSaveIniFile::~AutoSaveIniFile()
{
    IniFile::Save(m_LoadedFileName);
}

