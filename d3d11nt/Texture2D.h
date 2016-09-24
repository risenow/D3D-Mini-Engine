#pragma once
#include <d3d11.h>
#include "GraphicsDevice.h"
#include "GraphicsOptions.h"

class Texture2D
{
public:
    Texture2D();
    Texture2D(ID3D11Texture2D* texture);
    Texture2D(GraphicsDevice& device, size_t width, size_t height, 
              unsigned int mipLevels, unsigned int arraySize, DXGI_FORMAT dxgiFormat, 
              DXGI_SAMPLE_DESC sampleDesc, D3D11_USAGE usage, UINT bindFlags,
              UINT cpuAccessFlags, UINT miscFlags, D3D11_SUBRESOURCE_DATA* initialData = nullptr);

    void Resize(GraphicsDevice& device, size_t width, size_t height);

    size_t GetWidth() const;
    size_t GetHeight() const;
    size_t GetConsumedVideoMemorySize();

    ID3D11Texture2D* GetD3D11Texture2D() const;

    bool IsValid() const;

    void Release();
private:
    void FillDesc(D3D11_TEXTURE2D_DESC& desc);
    size_t GetBytesPerPixelForDXGIFormat(DXGI_FORMAT format);

    size_t m_Width;
    size_t m_Height;
    unsigned int m_MipLevels;
    unsigned int m_ArraySize;
    DXGI_FORMAT m_DXGIFormat;
    DXGI_SAMPLE_DESC m_SampleDesc;
    D3D11_USAGE m_Usage;
    UINT m_BindFlags;
    UINT m_CPUAccessFlags;
    UINT m_MiscFlags;

    ID3D11Texture2D* m_D3D11Texture;
};

class Texture2DHelper 
{
public:
    static Texture2D CreateCommonTexture(GraphicsDevice& device, size_t width, size_t height, 
                                         unsigned int mipLevels, DXGI_FORMAT format, MultisampleType multisample,
                                         UINT bindFlags, D3D11_SUBRESOURCE_DATA* initialData = nullptr);
};