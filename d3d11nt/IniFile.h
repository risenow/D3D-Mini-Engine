#pragma once
#include "stlcontainersintegration.h"
#include <map>
#include <vector>
#include <string>

struct IniParseState 
{
    IniParseState();

    bool m_IsSectionLine;
    std::string m_CurrentSection;
};

#define popDeclareGetValParams const std::string& section, const std::string& name

class IniFile
{
public:
    IniFile();
    IniFile(const std::string& fileName);
    
    template<typename T>
    void SetValue(popDeclareGetValParams, const T& value)
    {
        m_Ini[section][name] = { std::to_string(value) };
    }
    template<>
    void SetValue<std::string>(popDeclareGetValParams, const std::string& value)
    {
        m_Ini[section][name] = { value };
    }

    std::string      GetValue           (popDeclareGetValParams, const std::string& defaultValue) const;
    int              GetIntValue        (popDeclareGetValParams, int defaultValue) const;
    unsigned int     GetUIntValue       (popDeclareGetValParams, unsigned int defaultValue) const;
    float            GetFloatValue      (popDeclareGetValParams, float defaultValue) const;
    
    void GetValues(popDeclareGetValParams, const STLVector<std::string>& defaultValues, STLVector<std::string>& values);

    void Load(const std::string& fileName);
    void Save(const std::string& fileName);
private:
    std::string ReadSectionFromLine(const std::string& ln);
    void ReadPropertyFromLine(const std::string& ln);
    std::string ReadPropertyNameFromLine(const std::string& ln, unsigned int equalSignPos);
    std::string ReadPropertyValuesAsWholeStringFromLine(const std::string& ln, unsigned int equalSignPos);
    void DecomposeWholeStringPropertyValues(const std::string& wholeStringPropertyValues, STLVector<std::string>& values);
    bool IsCommentStartChar(char c);

    STLVector<std::string>* GetValues(popDeclareGetValParams) const;

    std::string m_CurrentSection;
    bool m_AtLeastOneSectionRead;
    typedef STLMap<std::string, STLVector<std::string>> IniProperties;
    typedef STLMap<std::string, IniProperties> IniStructure;
    IniStructure m_Ini;
};