#include "stdafx.h"
#include "GraphicsSurface.h"
#include "logicsafety.h"

GraphicsSurface::GraphicsSurface(): m_RenderTargetView(nullptr), m_DepthStencilView(nullptr) {}
GraphicsSurface::GraphicsSurface(GraphicsDevice& device, Texture2D* texture, GraphicsSurfaceType type)
                                : m_Texture(texture), m_RenderTargetView(nullptr), m_DepthStencilView(nullptr)
{
    switch (type)
    {
    case GRAPHICS_SURFACE_TYPE_RENDER_TARGET:
        D3D_HR_OP(device.GetD3D11Device()->CreateRenderTargetView(texture->GetD3D11Texture2D(), nullptr, &m_RenderTargetView));
        break;
    case GRAPHICS_SURFACE_TYPE_DEPTH_STENCIL:
        D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilView(texture->GetD3D11Texture2D(), nullptr, &m_DepthStencilView));
        break;
    default:
        popAssert(false);
        break;
    }
}
GraphicsSurface::~GraphicsSurface()
{
}

void GraphicsSurface::Resize(GraphicsDevice& device, size_t width, size_t height)
{
    popAssert(m_Texture);
    popAssert(m_RenderTargetView || m_DepthStencilView);

    m_Texture->Resize(device, width, height);
    
    if (m_RenderTargetView)
        D3D_HR_OP(device.GetD3D11Device()->CreateRenderTargetView(m_Texture->GetD3D11Texture2D(), nullptr, &m_RenderTargetView));
    if (m_DepthStencilView)
        D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilView(m_Texture->GetD3D11Texture2D(), nullptr, &m_DepthStencilView));
}

size_t GraphicsSurface::GetWidth() const
{
    return m_Texture->GetWidth();
}
size_t GraphicsSurface::GetHeight() const
{
    return m_Texture->GetHeight();
}

ID3D11RenderTargetView* GraphicsSurface::GetRenderTargetView() const
{
    return m_RenderTargetView;
}
ID3D11DepthStencilView* GraphicsSurface::GetDepthStencilView() const
{
    return m_DepthStencilView;
}

bool GraphicsSurface::IsValid() const
{
    return (m_RenderTargetView || m_DepthStencilView) && !(m_RenderTargetView && m_DepthStencilView);
}

void GraphicsSurface::Release()
{
    if (m_RenderTargetView)
    {
        m_RenderTargetView->Release();
        m_RenderTargetView = nullptr;
    }
    if (m_DepthStencilView)
    {
        m_DepthStencilView->Release();
        m_DepthStencilView = nullptr;
    }
}