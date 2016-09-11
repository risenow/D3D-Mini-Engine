#pragma once

template<typename T>
struct NumberRange
{
public:
    NumberRange(T from, T howMuch) : m_Start(from), m_End(from + howMuch) {}
    bool InRange(T v) { return (v >= m_Start && v <= m_End);  }

    T GetStart() { return m_Start; }
    T GetEnd() { return m_End; }
private:
    T m_Start;
    T m_End;
};