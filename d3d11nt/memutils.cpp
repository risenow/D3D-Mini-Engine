#include "stdafx.h"
#include "memutils.h"

void* align(void* p, unsigned int al, unsigned int& offset)
{
    offset = 0;
    unsigned int t = (unsigned int)p;
    unsigned int lastBits = (t&(al - 1));
    if (lastBits)
    {
        offset = (al - lastBits);
        t = t + offset;
    }
    return (void*)t;
}