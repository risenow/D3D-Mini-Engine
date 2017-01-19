#pragma once
#include <string>

#define popElementsNum(a) (sizeof(a)/sizeof(a[0]))

typedef unsigned char* uptr;

void* align(void* p, unsigned int al, unsigned int& offset);

#define MB *1024*1024

std::string BytesNumberToFormattedString(size_t size);