#pragma once
#include "System/stlcontainersintegration.h"
#include <dxgi.h>
#include <vector>

class DisplayAdaptersList
{
public:
    DisplayAdaptersList();
    ~DisplayAdaptersList();

    IDXGIAdapter* GetAdapter(unsigned int i);

    size_t GetAdaptersCount();
    void LogInfo();
private:
    STLVector<IDXGIAdapter*> m_Adapters;
};