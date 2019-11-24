#pragma once

#include "System/stlcontainersintegration.h"
#include <limits>

size_t clamp(size_t x, size_t min, size_t max);

double logab(double b, double a);

template<class T>
struct NumberRange;

template<class T>
STLVector<T> GetAllNumbersFromRange(const NumberRange<T>& numberRange);

template<class T>
void GetAllNumbersFromRange(const NumberRange<T>& numberRange, STLVector<T>& outNumbers);

template<typename T>
struct NumberRange
{
public:
    NumberRange(T from, T howMuch) : m_Start(from), m_End(from + howMuch) {}
    bool InRange(T v) const { return (v >= m_Start && v <= m_End);  }

    T GetStart() const { return m_Start; }
    T GetEnd() const { return m_End; }

    STLVector<T> GetAllNumbers() const
    {
        return GetAllNumbersFromRange(*this);
    }
    void GetAllNumbers(STLVector<T>& outNumbers) const
    {
        GetAllNumbersFromRange(*this, outNumbers);
    }

    static NumberRange<T> GetFullRange()
    {
        return NumberRange<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    }
private:
    T m_Start;
    T m_End;
};

template<class T>
STLVector<T> GetAllNumbersFromRange(const NumberRange<T>& numberRange)
{
    STLVector<T> outNumbers(numberRange.GetEnd() - numberRange.GetStart() + 1);
    for (T i = numberRange.GetStart(); i <= numberRange.GetEnd(); i++)
        outNumbers[i - numberRange.GetStart()] = i;
    return outNumbers;
}
template<class T>
void GetAllNumbersFromRange(const NumberRange<T>& numberRange, STLVector<T>& outNumbers)
{
    outNumbers.resize(numberRange.GetEnd() - numberRange.GetStart() + 1);
    for (T i = numberRange.GetStart(); i <= numberRange.GetEnd(); i++)
        outNumbers[i - numberRange.GetStart()] = i;
}