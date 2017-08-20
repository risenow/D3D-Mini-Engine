#pragma once
#include <string>
#include <iostream>
#include "System/timeutils.h"
#include "System/ProfileUnit.h"

class Benchmark
{
public:
    Benchmark() {}
    Benchmark(const std::string& name) : Benchmark() 
    {
        m_Name = name;
    }
    virtual ~Benchmark() {}

    virtual void Run() = 0;

    void WriteResultToConsole()
    {
        std::cout << m_Name << "benchmark result: " << m_ProfileUnit.GetResultTime() << std::endl;
    }
    unsigned int GetResultTime() const { return m_ProfileUnit.GetResultTime(); }
protected:
    void BeginMeasure()
    {
		m_ProfileUnit.BeginProfile();
    }
    void EndMeasure()
    {
		m_ProfileUnit.EndProfile();
    }

    std::string m_Name;
	ProfileUnit m_ProfileUnit;
};