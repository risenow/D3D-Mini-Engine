#include "stdafx.h"
#include "Graphics/RenderSet.h"
#include "Texture2D.h"
#include "System/logicsafety.h"
#include "Graphics/dxlogicsafety.h"

RenderSet::RenderSet() {}

RenderSet::RenderSet(const std::vector<ColorSurface*>& colorSurfaces, const DepthSurface* depthStencilSurface) 
                   : m_ColorSurfaces(colorSurfaces), m_DepthStencilSurface((DepthSurface*)depthStencilSurface), m_ResourceIsOwned(false)
{
    CorrectnessGuard();
}

RenderSet::RenderSet(GraphicsDevice& device, size_t w, size_t h, MultisampleType ms, const std::vector<DXGI_FORMAT>& formats, bool uav) : m_ResourceIsOwned(true)
{
    m_DepthStencilTexture = Texture2DHelper::CreateCommonTexture(device,
        w,
        h,
        1,
        DXGI_FORMAT_R24G8_TYPELESS,//DXGI_FORMAT_D24_UNORM_S8_UINT,
        ms,
        D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
    m_DepthStencilSurface = popNew(DepthSurface)(device, &m_DepthStencilTexture);

    m_ColorSurfaces.resize(formats.size());
    m_RenderTargetTextures.resize(formats.size());
    
    uint32_t bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    if (uav)
        bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    for (size_t i = 0; i < formats.size(); i++)
    {
        m_RenderTargetTextures[i] = Texture2DHelper::CreateCommonTexture(device,
            w,
            h,
            1,
            formats[i],
            ms,
            bindFlags);
        m_ColorSurfaces[i] = popNew(ColorSurface)(device, &m_RenderTargetTextures[i]);
    }
}

size_t RenderSet::GetWidth() const
{
    assert(m_ColorSurfaces.size() || m_DepthStencilSurface);
    return (m_ColorSurfaces.size()) ? m_ColorSurfaces[0]->GetWidth() : m_DepthStencilSurface->GetWidth();
}
size_t RenderSet::GetHeight() const
{
    assert(m_ColorSurfaces.size() || m_DepthStencilSurface);
    return (m_ColorSurfaces.size()) ? m_ColorSurfaces[0]->GetHeight() : m_DepthStencilSurface->GetHeight();
}

void RenderSet::Set(GraphicsDevice& device)
{
    CorrectnessGuard();

    std::vector<ID3D11RenderTargetView*> renderTargetViews;
    renderTargetViews.reserve(m_ColorSurfaces.size());
    for (size_t i = 0; i < m_ColorSurfaces.size(); i++)
        renderTargetViews.push_back(m_ColorSurfaces[i]->GetView());
    device.GetD3D11DeviceContext()->OMSetRenderTargets(renderTargetViews.size(), renderTargetViews.data(),
                                                          m_DepthStencilSurface ? m_DepthStencilSurface->GetView() : nullptr);
}

void RenderSet::CorrectnessGuard()
{
#ifdef _DEBUG
    bool correctColorView = false;
    for (ColorSurface*& surface : m_ColorSurfaces)
        if (surface->GetView())
            correctColorView = true;
    popAssert(correctColorView || (m_DepthStencilSurface && m_DepthStencilSurface->GetView()));
    if (correctColorView && m_DepthStencilSurface && m_DepthStencilSurface->GetView())
    {
        for (size_t i = 0; i < m_ColorSurfaces.size(); i++)
            popAssert(m_ColorSurfaces[i]->GetWidth()  == m_DepthStencilSurface->GetWidth() &&
                        m_ColorSurfaces[i]->GetHeight() == m_DepthStencilSurface->GetHeight());
    }
#endif
}
void RenderSet::Clear(GraphicsDevice& device, float color[4])
{
    CorrectnessGuard();

    ID3D11DeviceContext* context = device.GetD3D11DeviceContext();
    for (size_t i = 0; i < m_ColorSurfaces.size(); i++)
        if (m_ColorSurfaces[i]->GetTexture())
            context->ClearRenderTargetView(m_ColorSurfaces[i]->GetView(), color);
    if (m_DepthStencilSurface->GetTexture())
        context->ClearDepthStencilView(m_DepthStencilSurface->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}