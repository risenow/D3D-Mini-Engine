#include "stdafx.h"
#include "System/helperutil.h"

size_t clamp(size_t x, size_t min, size_t max)
{
    return std::min(std::max(x, min), max);
}

double logab(double b, double a)
{
    double res = 0;

    res = log(a) / log(b);
    return res;
}