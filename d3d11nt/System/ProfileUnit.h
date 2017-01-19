#pragma once
#include <Windows.h>
#include <string>

static const unsigned int INVALID_ID = (unsigned int)(-1);

class ProfileUnit
{
public:
    ProfileUnit();
    ProfileUnit(const std::string& name);

    void BeginProfile();
    void EndProfile();

    std::string GetName();
    long long GetResultTime();
protected:
    long long m_StartTime;
    long long m_ResultTime;
    std::string m_Name;
};

class ManagedProfileUnit : public ProfileUnit
{
public:
    typedef unsigned int IDType;

    ManagedProfileUnit(const std::string& name, IDType ID, IDType parentID);

    void SetParentID(IDType parentID);
    IDType GetID() const;
    IDType GetParentID() const;
private:
    IDType m_ID;
    IDType m_ParentID;
};

class AutoManagedProfileUnit
{
public:
    AutoManagedProfileUnit(const std::string& name);
    ~AutoManagedProfileUnit();
private:
    ManagedProfileUnit::IDType m_ManagedProfileUnitID;
};

#if (0)//def _DEBUG
#define popProfile(name) AutoManagedProfileUnit name##Profile(#name)
#else
#define popProfile(name)
#endif