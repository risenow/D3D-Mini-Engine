#include "stdafx.h"
#include "DisplayAdaptersList.h"
#include "Log.h"
#include "logicsafety.h"
#include <cstring>

DisplayAdaptersList::DisplayAdaptersList()
{
    IDXGIFactory* pDXGIFactory = nullptr;
    D3D_HR_OP(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory));
    IDXGIAdapter* currentAdapter = nullptr;
    UINT i = 0;
    while (pDXGIFactory->EnumAdapters(i, &currentAdapter) == S_OK)
    {
        m_Adapters.push_back(currentAdapter);
        i++;
    }
    pDXGIFactory->Release();

    LogInfo();
}

IDXGIAdapter* DisplayAdaptersList::GetAdapter(unsigned int i)
{
    popAssert(i < m_Adapters.size());
    return m_Adapters[i];
}
size_t DisplayAdaptersList::GetAdaptersCount()
{
    return m_Adapters.size();
}

void DisplayAdaptersList::LogInfo()
{
    LOG(std::string("Adapters enumerated:"));
    for (int i = 0; i < m_Adapters.size(); i++)
    {
        DXGI_ADAPTER_DESC currentAdapterDesc;
        D3D_HR_OP(m_Adapters[i]->GetDesc(&currentAdapterDesc));
        LOG("Adapter index: " + std::to_string(i));
        std::wstring t = std::wstring(L"Description: ") + std::wstring(currentAdapterDesc.Description);
        LOG(std::wstring(L"Description: ") + std::wstring(currentAdapterDesc.Description));
        LOG(std::string(""));
    }
}

