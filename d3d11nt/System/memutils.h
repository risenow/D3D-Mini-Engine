#pragma once
#include <string>
#include <thread>
#include <map>

#define popElementsNum(a) (sizeof(a)/sizeof(a[0]))

typedef unsigned char* uptr;

void* align(void* p, unsigned int al, unsigned int& offset);

#define MB *1024*1024
#define KB *1024
#define BYTES

std::string BytesNumberToFormattedString(size_t size); // should it even be in memutils?

#define DECLARE_THREAD_LOCAL_ALLOCATOR(name, type) \
thread_local ##type * g_##name;
