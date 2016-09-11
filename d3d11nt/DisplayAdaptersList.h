#pragma once
#include "stlcontainersintegration.h"
#include <dxgi.h>
#include <vector>

class DisplayAdaptersList
{
public:
    DisplayAdaptersList();

    IDXGIAdapter* GetAdapter(unsigned int i);

    size_t GetAdaptersCount();
    void LogInfo();
private:
    STLVector<IDXGIAdapter*> m_Adapters;
};