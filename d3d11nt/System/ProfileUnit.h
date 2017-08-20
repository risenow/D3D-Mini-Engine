#pragma once
#include <Windows.h>
#include <string>

class ProfileUnit
{
public:
    ProfileUnit();
    ProfileUnit(const std::string& name);

    void BeginProfile();
    void EndProfile();

    std::string GetName() const;
    long long GetResultTime() const;
protected:
    long long m_StartTime;
    long long m_ResultTime;
    std::string m_Name;
};

class ManagedProfileUnit : public ProfileUnit
{
public:
    typedef unsigned long IDType;

    ManagedProfileUnit(const std::string& name, IDType ID, IDType parentID);

    void SetParentID(IDType parentID);
    IDType GetID() const;
    IDType GetParentID() const;
private:
    IDType m_ID;
    IDType m_ParentID;
};

static const ManagedProfileUnit::IDType INVALID_ID = (ManagedProfileUnit::IDType)(-1);

class AutoManagedProfileUnit
{
public:
    AutoManagedProfileUnit(const std::string& name);
    ~AutoManagedProfileUnit();
private:
    ManagedProfileUnit::IDType m_ManagedProfileUnitID;
};

#ifdef INTEGRATE_PROFILE_SYSTEM
#define popProfile(name) AutoManagedProfileUnit name##Profile(#name)
#else
#define popProfile(name)
#endif