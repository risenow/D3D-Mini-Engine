#include "stdafx.h"
#include "System/memutils.h"

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

std::string BytesNumberToFormattedString(size_t size)
{
    size_t megabytes = size / 1024 / 1024;
    size_t bytesInMegabytes = megabytes * 1024 * 1024;
    size_t kilobytes = (size - bytesInMegabytes)/1024;
    size_t bytes = (size - bytesInMegabytes - kilobytes*1024);

    return std::to_string(megabytes) + "M " + std::to_string(kilobytes) + "K " + std::to_string(bytes) + "B";
}