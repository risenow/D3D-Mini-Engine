#pragma once
#include "System/IniFile.h"
#include "System/stlcontainersintegration.h"
#include "System/helperutil.h"
#include <vector>

//PROPERTY ARCHITECTURE QUESTION: CAN WE USE TEMPLATES?

class IniProperty
{
public:
    IniProperty(const std::string section, const std::string& key);

    virtual void SerializeTo(IniFile& ini) = 0;
    virtual void DeserializeFrom(const IniFile& ini) = 0;
protected:
    std::string m_Section;
    std::string m_Key;
};

//default value must have type INT?
class IniIntProperty : public IniProperty
{
public:
    IniIntProperty(const std::string& section, const std::string& key, int default, int* val, const NumberRange<int>& acceptableRange);

    virtual void SerializeTo(IniFile& ini);
    virtual void DeserializeFrom(const IniFile& ini);
private:
    NumberRange<int> m_AcceptableRange;
    int* m_Val;
    int m_Default;
};

template<class T>
class IniNumberProperty : public IniProperty
{
public:
    IniNumberProperty(const std::string& section, const std::string& key, 
                      T default, T* val, const NumberRange<T>& acceptableRange = NumberRange<T>::GetFullRange())
        : IniProperty(section, key), m_Default(default), m_Val(val), m_AcceptableRange(acceptableRange)
    {}

    virtual void SerializeTo(IniFile& ini)
    {
        ini.SetValue(m_Section, m_Key, *m_Val);
    }
    virtual void DeserializeFrom(const IniFile& ini)
    {
        T tempVal = ini.GetValue(m_Section, m_Key, m_Default);
        (*m_Val) = m_AcceptableRange.InRange(tempVal) ? tempVal : m_Default;
    }
private:
    NumberRange<T> m_AcceptableRange;
    T* m_Val;
    T m_Default;
};

//template this class to get rid of the type conversion(enum -> int; int -> enum) ?
class IniEnumProperty : public IniProperty
{
public:
    IniEnumProperty(const std::string& section, const std::string& key, int default, int* val, const STLVector<int>& acceptableValues);

    virtual void SerializeTo(IniFile& ini);
    virtual void DeserializeFrom(const IniFile& ini);
private:
    bool IsAcceptable(int val);

    STLVector<int> m_AcceptableValues;
    int* m_Val;
    int m_Default;
};

class IniStringProperty : public IniProperty
{
public:
    IniStringProperty(const std::string& section, const std::string& key, const std::string& default, std::string* val);

    virtual void SerializeTo(IniFile& ini);
    virtual void DeserializeFrom(const IniFile& ini);
private:
    std::string* m_Val;
    std::string m_Default;
};

class IniSerializable
{
public:
    virtual ~IniSerializable();

    void DeserializeFromIni(const IniFile& iniInfo);
    void SerializeToIni(IniFile& iniInfo);

    virtual void InitializeIniProperties() = 0;
protected:
    void AddProperty(IniProperty* iniProperty);

    STLVector<IniProperty*> m_Properties;

private:
    bool PropertiesIntitialized();
};