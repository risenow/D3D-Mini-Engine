#include "stdafx.h"
#include "IniFile.h"
#include "logicsafety.h"
#include <algorithm>
#include <iostream>
#include <fstream>

IniParseState::IniParseState() : m_IsSectionLine(false), m_CurrentSection("")
{}

IniFile::IniFile() {}
IniFile::IniFile(const std::string& fileName)
{
    Load(fileName);
}
void IniFile::Load(const std::string& fileName)
{
    m_CurrentSection = "";
    m_AtLeastOneSectionRead = false;

    std::ifstream iniFile(fileName);
    std::string s;
    bool eof = false;
    while (!eof)
    {
        eof = std::getline(iniFile, s).eof();
        if (!s.size())
            continue;
        if (s[0] == ';' || s[0] == '#')
            continue;
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
        for (IniProperties::iterator j = i->second.begin(); j != i->second.end(); i++)
        {
            file << j->first << '='; //name
            if (j->second.size() == 0) //no values
            {
                file << '\n';
                continue;
            }
            if (j->second.size() == 1) //one value
            {
                file << j->second[0] << '\n';
                continue;
            }
            file << j->second[0] << ',';                //multivalue
            for (int k = 1; k < j->second.size(); k++) 
            {
                file << ',' << j->second[k];
            }
            file << '\n';
        }
    }
    file.close();
}

int IniFile::GetIntValue(popDeclareGetValParams, int defaultValue) const
{
    return std::stoi(GetValue(section, name, std::to_string(defaultValue)));
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
    return std::stof(GetValue(section, name, std::to_string(defaultValue)));
}

std::string IniFile::GetValue(popDeclareGetValParams, const std::string& defaultValue) const
{
    std::vector<std::string>* values = GetValues(section, name);
    if (!values || !(*values).size())
        return defaultValue;

    return (*values)[0];
}

//public
void IniFile::GetValues(popDeclareGetValParams, const std::vector<std::string>& defaultValues, std::vector<std::string>& outValues)
{
    popAssert(defaultValues.size());
    std::vector<std::string>* values = GetValues(section, name);
    if (!values || (*values).size())
    {
        outValues = defaultValues;
        return;
    }

    outValues = *values;
}

//private method for internal use only
std::vector<std::string>* IniFile::GetValues(popDeclareGetValParams) const
{
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
    return (std::vector<std::string>*)&propertyIter->second;
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
    popAssert(ln[charIndex] != ']');
    return ln.substr(1, charIndex - 2); //exclusive [ & ]  //-2 because first param is offset
}
void IniFile::ReadPropertyFromLine(const std::string& ln)
{
    std::vector<std::string> propertyValues;

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

void IniFile::DecomposeWholeStringPropertyValues(const std::string& wholeStringPropertyValues, std::vector<std::string>& values)
{
    unsigned int valuesNum = std::count(wholeStringPropertyValues.begin(), wholeStringPropertyValues.end(), ',') + 1;
    values.resize(valuesNum);

    std::string tempValueString;
    int j = 0;
    for (int i = 0; i < wholeStringPropertyValues.size(); i++)
    {
        if (wholeStringPropertyValues[i] == ',')
        {
            values[j] = tempValueString;
            tempValueString = "";
            j++;
        }
        tempValueString += wholeStringPropertyValues[i];
    }
    values[j] = tempValueString;
}
