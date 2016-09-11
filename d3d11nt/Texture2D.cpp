#include "stdafx.h"
#include "Texture2D.h"
#include "multisampleutils.h"
#include "logicsafety.h"

Texture2D::Texture2D() : m_D3D11Texture(nullptr) {}
Texture2D::Texture2D(ID3D11Texture2D* texture) : m_D3D11Texture(texture)
{
    D3D11_TEXTURE2D_DESC desc;
    m_D3D11Texture->GetDesc(&desc);
    m_Width = desc.Width;
    m_Height = desc.Height;
    m_ArraySize = desc.ArraySize;
    m_MipLevels = desc.MipLevels;
    m_DXGIFormat = desc.Format;
    m_BindFlags = desc.BindFlags;
    m_CPUAccessFlags = desc.CPUAccessFlags;
    m_MiscFlags = desc.MiscFlags;
    m_Usage = desc.Usage;
    m_SampleDesc = desc.SampleDesc;
}
Texture2D::Texture2D(GraphicsDevice& device, size_t width, size_t height, unsigned int mipLevels, 
                     unsigned int arraySize, DXGI_FORMAT dxgiFormat, 
                     DXGI_SAMPLE_DESC sampleDesc, D3D11_USAGE usage, 
                     UINT bindFlags, UINT cpuAccessFlags,
                     UINT miscFlags, D3D11_SUBRESOURCE_DATA* initialData)
                     : m_Width(width), m_Height(height), m_MipLevels(mipLevels),
                       m_ArraySize(arraySize), m_DXGIFormat(dxgiFormat),
                       m_SampleDesc(sampleDesc), m_Usage(usage),
                       m_BindFlags(bindFlags), m_CPUAccessFlags(cpuAccessFlags),
                       m_MiscFlags(miscFlags)
{
    D3D11_TEXTURE2D_DESC desc;
    FillDesc(desc);

    D3D_HR_OP(device.GetD3D11Device()->CreateTexture2D(&desc, initialData, &m_D3D11Texture));
}

void Texture2D::Resize(GraphicsDevice& device, size_t width, size_t height)
{
    if (m_Width != width || m_Height != height)
    {
        m_D3D11Texture->Release();
        m_Width = width;
        m_Height = height;
        D3D11_TEXTURE2D_DESC desc;
        FillDesc(desc);
        D3D_HR_OP(device.GetD3D11Device()->CreateTexture2D(&desc, nullptr, &m_D3D11Texture));
    }
}

size_t Texture2D::GetWidth() const
{
    return m_Width;
}
size_t Texture2D::GetHeight() const
{
    return m_Height;
}

ID3D11Texture2D* Texture2D::GetD3D11Texture2D() const
{
    return m_D3D11Texture;
}
void Texture2D::FillDesc(D3D11_TEXTURE2D_DESC& desc)
{
    desc.Width = m_Width;
    desc.Height = m_Height;
    desc.MipLevels = m_MipLevels;
    desc.ArraySize = m_ArraySize;
    desc.Format = m_DXGIFormat;
    desc.SampleDesc = m_SampleDesc;
    desc.Usage = m_Usage;
    desc.BindFlags = m_BindFlags;
    desc.CPUAccessFlags = m_CPUAccessFlags;
    desc.MiscFlags = m_MiscFlags;
}

bool Texture2D::IsValid() const
{
    return m_D3D11Texture != nullptr;
}

void Texture2D::Release() 
{
    m_D3D11Texture->Release();
    m_D3D11Texture = nullptr;
}

Texture2D Texture2DHelper::CreateCommonTexture(GraphicsDevice& device, size_t width, size_t height,
                                               unsigned int mipLevels, DXGI_FORMAT format,
                                               MultisampleType multisample, UINT bindFlags, 
                                               D3D11_SUBRESOURCE_DATA* initialData)
{
    return Texture2D(device, width, height, mipLevels, 1, format, GetSampleDesc(device, format, multisample), D3D11_USAGE_DEFAULT, bindFlags, 0, 0, initialData);
}