#include "stdafx.h"
#include "ProfileUnitsManager.h"
#include <limits>

ProfileUnitsManager::ProfileUnitsManager() {}
ProfileUnitsManager& ProfileUnitsManager::GetInstance()
{
    static ProfileUnitsManager instance;
    return instance;
}
ManagedProfileUnit::IDType ProfileUnitsManager::CreateProfileUnit(const std::string& name)
{
    PerThreadProfileUnitsData& perThreadData = GetDataForThisThread();
    ManagedProfileUnit::IDType newID = perThreadData.m_Units.size();
    perThreadData.m_Units.push_back(ManagedProfileUnit(name, newID, INVALID_ID));
    return newID;
}
void ProfileUnitsManager::BeginProfile(ManagedProfileUnit::IDType profileUnitID)
{
    PerThreadProfileUnitsData& perThreadData = GetDataForThisThread();
    ManagedProfileUnit& profileUnit = perThreadData.m_Units[profileUnitID];
    profileUnit.SetParentID(perThreadData.m_IDs.size() ? perThreadData.m_IDs.top() : INVALID_ID);
    perThreadData.m_IDs.push(profileUnit.GetID());
    profileUnit.BeginProfile();
}
void ProfileUnitsManager::EndProfile(ManagedProfileUnit::IDType profileUnitID)
{
    PerThreadProfileUnitsData& perThreadData = GetDataForThisThread();
    ManagedProfileUnit& profileUnit = perThreadData.m_Units[profileUnitID];
    profileUnit.EndProfile();
    perThreadData.m_IDs.pop();
}
ProfileUnitsManager::PerThreadProfileUnitsData& ProfileUnitsManager::GetDataForThisThread()
{
    return m_PerThreadData[std::this_thread::get_id()];
}
ProfileStats ProfileUnitsManager::GatherStatsForProfileWithName(const std::string& name)
{
    PerThreadProfileUnitsData& perThreadData = GetDataForThisThread();

    if (!perThreadData.m_Units.size())
        return ProfileStats();

    long long summ = 0;
    unsigned long count = 0;
    long long minTime = std::numeric_limits<long long>::max();
    long long maxTime = 0;
    ManagedProfileUnit::IDType minTimeProfileID = std::numeric_limits<unsigned long>::max();
    ManagedProfileUnit::IDType maxTimeProfileID = std::numeric_limits<unsigned long>::max();
    for (long i = 0; i < perThreadData.m_Units.size(); i++)
    {
        ManagedProfileUnit& currentProfileUnit = perThreadData.m_Units[i];
        if (currentProfileUnit.GetName() == name)
        {
            long long resultTime = currentProfileUnit.GetResultTime();
            if (resultTime < minTime)
            {
                minTime = resultTime;
                minTimeProfileID = i;
            }
            if (resultTime > maxTime)
            {
                maxTime = resultTime;
                maxTimeProfileID = i;
            }
            summ += resultTime; //potentially summ variable overflow
            count++;
        }
    }
    return ProfileStats(minTime, minTimeProfileID, maxTime, maxTimeProfileID, (double)summ / (double)count, count);
}