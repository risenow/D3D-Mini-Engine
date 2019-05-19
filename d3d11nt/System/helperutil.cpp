#include "stdafx.h"
#include "System/helperutil.h"

size_t clamp(size_t x, size_t min, size_t max)
{
    return std::min(std::max(x, min), max);
}