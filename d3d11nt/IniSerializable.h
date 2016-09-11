#pragma once
#include "IniFile.h"
#include "helperutil.h"
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

//template this class to get rid of the type conversion(enum -> int; int -> enum) ?
class IniEnumProperty : public IniProperty
{
public:
    IniEnumProperty(const std::string& section, const std::string& key, int default, int* val, const std::vector<int>& acceptableValues);

    virtual void SerializeTo(IniFile& ini);
    virtual void DeserializeFrom(const IniFile& ini);
private:
    bool IsAcceptable(int val);

    std::vector<int> m_AcceptableValues;
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

    virtual void DeserializeFromIni(const IniFile& iniInfo) = 0;
    virtual void SerializeToIni(IniFile& iniInfo) = 0;
protected:
    void AddProperty(IniProperty* iniProperty);

    void BasicSerializeTo(IniFile& iniInfo);
    void BasicDeserializeFrom(const IniFile& iniInfo);

    std::vector<IniProperty*> m_Properties;
};