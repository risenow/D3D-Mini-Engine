#pragma once
#include <string>
#include "System/stlcontainersintegration.h"
#include <thread>
#include <stack>
#include "ProfileUnit.h"

struct ProfileStats
{
    ProfileStats() : m_MinTime(0), m_MaxTime(0), m_AverageTime(0.0), m_NumOccurences(0) {}
    ProfileStats(long long minTime, ManagedProfileUnit::IDType minTimeProfileID, 
                long long maxTime, ManagedProfileUnit::IDType maxTimeProfileID, 
                double averageTime, unsigned int numOccurences) : m_MinTime(minTime), m_MinTimeProfileID(minTimeProfileID),
                                                                  m_MaxTime(maxTime), m_MaxTimeProfileID(maxTimeProfileID),
                                                                  m_AverageTime(averageTime), m_NumOccurences(numOccurences) {}

    ManagedProfileUnit::IDType m_MinTimeProfileID;
    ManagedProfileUnit::IDType m_MaxTimeProfileID;
    long long m_MinTime;
    long long m_MaxTime;
    double m_AverageTime;
    unsigned int m_NumOccurences;
};

class ProfileUnitsManager
{
public:
    static ProfileUnitsManager& GetInstance();

    ManagedProfileUnit::IDType CreateProfileUnit(const std::string& name);
    void BeginProfile(ManagedProfileUnit::IDType unitID);
    void EndProfile(ManagedProfileUnit::IDType unitID);

    ProfileStats GatherStatsForProfileWithName(const std::string& name);
private:
    struct PerThreadProfileUnitsData
    {
        STLStack<ManagedProfileUnit::IDType> m_IDs;
        STLVector<ManagedProfileUnit> m_Units;
    };

    PerThreadProfileUnitsData& GetDataForThisThread();
    ProfileUnitsManager();

    STLMap<std::thread::id, PerThreadProfileUnitsData> m_PerThreadData;
};

