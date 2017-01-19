#include "stdafx.h"
#include "System/IniSerializable.h"
#include "System/strutils.h"
#include "System/logicsafety.h"

IniProperty::IniProperty(const std::string section, const std::string& key) 
                         : m_Section(section), m_Key(key)
{}

IniIntProperty::IniIntProperty(const std::string& section, const std::string& key, 
                               int default, int* val, const NumberRange<int>& acceptableRange) 
                               : IniProperty(section, key), m_Default(default), m_Val(val), m_AcceptableRange(acceptableRange)
{}

void IniIntProperty::SerializeTo(IniFile& ini)
{
    ini.SetValue(m_Section, m_Key, *m_Val);
}
void IniIntProperty::DeserializeFrom(const IniFile& ini)
{
    int tempVal = ini.GetIntValue(m_Section, m_Key, m_Default);
    (*m_Val) = m_AcceptableRange.InRange(tempVal) ? tempVal : m_Default;
}

IniEnumProperty::IniEnumProperty(const std::string& section, const std::string& key,
                                 int default, int* val, const STLVector<int>& acceptableValues)
                                 : IniProperty(section, key), m_Default(default), m_Val(val), m_AcceptableValues(acceptableValues)
{}
bool IniEnumProperty::IsAcceptable(int val)
{
    for (unsigned int i = 0; i < m_AcceptableValues.size(); i++)
    {
        if (val == m_AcceptableValues[i])
            return true;
    }
    return false;
}
void IniEnumProperty::DeserializeFrom(const IniFile& ini)
{
    popAssert(IsAcceptable(m_Default));
    int tempVal = ini.GetIntValue(m_Section, m_Key, m_Default);
    (*m_Val) = IsAcceptable(tempVal) ? tempVal : m_Default;
}
void IniEnumProperty::SerializeTo(IniFile& ini)
{
    ini.SetValue(m_Section, m_Key, *m_Val);
}

IniStringProperty::IniStringProperty(const std::string& section, const std::string& key, 
                                     const std::string& default, std::string* val) 
                                     : IniProperty(section, key), m_Default(default), m_Val(val)
{}
void IniStringProperty::DeserializeFrom(const IniFile& ini)
{
    (*m_Val) = ini.GetStrValue(m_Section, m_Key, m_Default);
}
void IniStringProperty::SerializeTo(IniFile& ini)
{
    ini.SetValue(m_Section, m_Key, (*m_Val));
}


IniSerializable::~IniSerializable()
{
    for (unsigned int i = 0; i < m_Properties.size(); i++)
    {
        popDelete(m_Properties[i]);
    }
}
void IniSerializable::DeserializeFromIni(const IniFile& ini)
{
    if (!PropertiesIntitialized())
        InitializeIniProperties();
    for (unsigned int i = 0; i < m_Properties.size(); i++)
    {
        m_Properties[i]->DeserializeFrom(ini);
    }
}
void IniSerializable::SerializeToIni(IniFile& ini)
{
    if (!PropertiesIntitialized())
        InitializeIniProperties();
    for (unsigned int i = 0; i < m_Properties.size(); i++)
    {
        m_Properties[i]->SerializeTo(ini);
    }

}
void IniSerializable::AddProperty(IniProperty* iniProperty)
{
    m_Properties.push_back(iniProperty);
}

bool IniSerializable::PropertiesIntitialized()
{
    return m_Properties.size() != 0;
}