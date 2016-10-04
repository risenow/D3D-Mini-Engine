#include "stdafx.h"
#include "GraphicsDevice.h"
#include "memutils.h"
#include "logicsafety.h"
#include <dxgidebug.h>

D3D11DeviceCreationFlags::D3D11DeviceCreationFlags() : m_Flags(0)
{}
D3D11DeviceCreationFlags::D3D11DeviceCreationFlags(bool createDebugDevice, bool createSinglethreadedDevice) : m_Flags(0)
{
    m_Flags = 0;
    if (createDebugDevice)
        m_Flags |= D3D11_CREATE_DEVICE_DEBUG;
    if (createSinglethreadedDevice)
        m_Flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
}
D3D11DeviceCreationFlags::D3D11DeviceCreationFlags(const CommandLineArgs& cmdLnArgs) : m_Flags(0)
{
    if (cmdLnArgs.HasArg(COMMAND_LINE_ARG_DEBUG_D3D_DEVICE_ENABLE))
        m_Flags |= D3D11_CREATE_DEVICE_DEBUG;
    if (cmdLnArgs.HasArg(COMMAND_LINE_ARG_SINGLETHREADED_DEVICE_ENABLE))
        m_Flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
}
UINT32 D3D11DeviceCreationFlags::Get()
{
    return m_Flags;
}


GraphicsDevice::GraphicsDevice() {}
GraphicsDevice::GraphicsDevice(D3D11DeviceCreationFlags deviceCreationFlags, AcceptableFeatureLevel acceptableFeatureLevel, IDXGIAdapter* adapter)
{
    AcceptableFeatureLevels featureLevels = GetFeatureLevels(acceptableFeatureLevel);
    
    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
    if (adapter)
        driverType = D3D_DRIVER_TYPE_UNKNOWN;

    D3D_HR_OP(D3D11CreateDevice(adapter,
                            driverType,
                            0,
                            deviceCreationFlags.Get(),
                            &featureLevels[0],
                            featureLevels.size(),
                            D3D11_SDK_VERSION,
                            &m_D3D11Device,
                            &m_D3D11DeviceFeatureLevel,
                            &m_D3D11DeviceContext));
}

GraphicsDevice::~GraphicsDevice()
{
    if (m_D3D11Device)
        m_D3D11Device->Release();
    if (m_D3D11DeviceContext)
        m_D3D11DeviceContext->Release();
}

STLVector<D3D_FEATURE_LEVEL> GraphicsDevice::GetFeatureLevels(AcceptableFeatureLevel acceptableFeatureLevel)
{
    AcceptableFeatureLevels featureLevels;
    switch (acceptableFeatureLevel)
    {
    case FEATURE_LEVEL_ONLY_D3D11:
        featureLevels = { D3D_FEATURE_LEVEL_11_0 };
        break;
    case FEATURE_LEVEL_ACTUAL:
        featureLevels = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
        break;
    case FEATURE_LEVEL_ALL:
        featureLevels = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3 };
        break;
    }
    return featureLevels;
}

ID3D11Device* GraphicsDevice::GetD3D11Device() const
{
    return m_D3D11Device;
}
ID3D11DeviceContext* GraphicsDevice::GetD3D11DeviceContext() const
{
    return m_D3D11DeviceContext;
}

void GraphicsDevice::ReportAllLiveObjects()
{
    typedef HRESULT(WINAPI * LPDXGIGETDEBUGINTERFACE)(REFIID, void **);

    static HMODULE dxgidebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dxgidebug)
    {
        auto dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
            reinterpret_cast<void*>(GetProcAddress(dxgidebug, "DXGIGetDebugInterface")));
        IDXGIDebug* dxgiDebug;
        dxgiGetDebugInterface(__uuidof(IDXGIDebug), (void**)&dxgiDebug);
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
    }
}