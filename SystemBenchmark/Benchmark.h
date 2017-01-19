#pragma once
#include <string>
#include <iostream>
#include "System/timeutils.h"

class Benchmark
{
public:
    Benchmark() : m_StartTime(0), m_ResultTime(0) {}
    Benchmark(const std::string& name) : Benchmark() 
    {
        m_Name = name;
    }
    virtual ~Benchmark() {}

    virtual void Run() = 0;

    void WriteResultToConsole()
    {
        std::cout << m_Name << "benchmark result: " << m_ResultTime << std::endl;
    }
    unsigned int GetResultTime() const { return m_ResultTime; }
protected:
    void StartMeasure()
    {
        m_StartTime = GetMilisecondsSinceProgramStart();
    }
    void EndMeasure()
    {
        m_ResultTime = GetMilisecondsSinceProgramStart() - m_StartTime;
    }

    std::string m_Name;

    unsigned int m_StartTime;
    unsigned int m_ResultTime;
};