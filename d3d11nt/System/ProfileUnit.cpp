#include "stdafx.h"
#include "ProfileUnit.h"
#include "ProfileUnitsManager.h"
#include "timeutils.h"

ProfileUnit::ProfileUnit() : m_StartTime(0) {}
ProfileUnit::ProfileUnit(const std::string& name) : m_StartTime(0), m_ResultTime(0), m_Name(name) {}
void ProfileUnit::BeginProfile()
{
    m_StartTime = GetPerfomanceCounterMicroseconds();
}
void ProfileUnit::EndProfile()
{
    m_ResultTime = GetPerfomanceCounterMicroseconds() - m_StartTime;
}
std::string ProfileUnit::GetName() const
{
    return m_Name;
}
long long ProfileUnit::GetResultTime() const
{
    return m_ResultTime;
}

ManagedProfileUnit::ManagedProfileUnit(const std::string& name, IDType ID, IDType parentID) : ProfileUnit(name),
                                                                                                          m_ID(ID), 
                                                                                                          m_ParentID(parentID)
{}
void ManagedProfileUnit::SetParentID(IDType parentID)
{
    m_ParentID = parentID;
}
ManagedProfileUnit::IDType ManagedProfileUnit::GetParentID() const
{
    return m_ParentID;
}
ManagedProfileUnit::IDType ManagedProfileUnit::GetID() const
{
    return m_ID;
}

AutoManagedProfileUnit::AutoManagedProfileUnit(const std::string& name)
{
    m_ManagedProfileUnitID = ProfileUnitsManager::GetInstance().CreateProfileUnit(name);
    ProfileUnitsManager::GetInstance().BeginProfile(m_ManagedProfileUnitID);
}
AutoManagedProfileUnit::~AutoManagedProfileUnit()
{
    ProfileUnitsManager::GetInstance().EndProfile(m_ManagedProfileUnitID);
}