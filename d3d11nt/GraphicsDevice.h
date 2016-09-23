#pragma once
#include <d3d11.h>
#include "stlcontainersintegration.h"
#include "CommandLineArgs.h"

enum AcceptableFeatureLevel
{
    FEATURE_LEVEL_ONLY_D3D11, //11
    FEATURE_LEVEL_ACTUAL, // 11, 10
    FEATURE_LEVEL_ALL // 11, 10, 9.3
};

class D3D11DeviceCreationFlags
{
public:
    D3D11DeviceCreationFlags();
    D3D11DeviceCreationFlags(bool createDebugDevice, bool createSinglethreadedDevice);
    D3D11DeviceCreationFlags(const CommandLineArgs& cmdLnArgs);

    UINT32 Get();
private:
    UINT32 m_Flags;
};

class GraphicsDevice
{
public:
    GraphicsDevice();
    GraphicsDevice(D3D11DeviceCreationFlags deviceCreationFlags, AcceptableFeatureLevel acceptableFeatureLevel, IDXGIAdapter* adapter = nullptr);

    ID3D11Device* GetD3D11Device() const;
    ID3D11DeviceContext* GetD3D11DeviceContext() const;
private:
    typedef STLVector<D3D_FEATURE_LEVEL> AcceptableFeatureLevels;
    AcceptableFeatureLevels GetFeatureLevels(AcceptableFeatureLevel acceptableFeatureLevel);

    ID3D11Device* m_D3D11Device;
    ID3D11DeviceContext* m_D3D11DeviceContext;
    D3D_FEATURE_LEVEL m_D3D11DeviceFeatureLevel;
};