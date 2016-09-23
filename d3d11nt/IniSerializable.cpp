#include "stdafx.h"
#include "IniSerializable.h"
#include "strutils.h"
#include "logicsafety.h"

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
    for (int i = 0; i < m_AcceptableValues.size(); i++)
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
    (*m_Val) = ini.GetValue(m_Section, m_Key, m_Default);
}
void IniStringProperty::SerializeTo(IniFile& ini)
{
    ini.SetValue(m_Section, m_Key, (*m_Val));
}


IniSerializable::~IniSerializable()
{
    for (int i = 0; i < m_Properties.size(); i++)
    {
        delete m_Properties[i];
    }
}
void IniSerializable::BasicDeserializeFrom(const IniFile& ini)
{
    for (int i = 0; i < m_Properties.size(); i++)
    {
        m_Properties[i]->DeserializeFrom(ini);
    }
}
void IniSerializable::BasicSerializeTo(IniFile& ini)
{
    for (int i = 0; i < m_Properties.size(); i++)
    {
        m_Properties[i]->SerializeTo(ini);
    }

}
void IniSerializable::AddProperty(IniProperty* iniProperty)
{
    m_Properties.push_back(iniProperty);
}