#include "stdafx.h"
#include "RenderSet.h"
#include "logicsafety.h"

RenderSet::RenderSet() {}

RenderSet::RenderSet(GraphicsSurface colorSurface, GraphicsSurface depthStencilSurface) 
                   : m_ColorSurface(colorSurface), m_DepthStencilSurface(depthStencilSurface) 
{
    popAssert(colorSurface.GetRenderTargetView());
    popAssert(!depthStencilSurface.GetRenderTargetView());
    if (depthStencilSurface.GetDepthStencilView())
    {
        popAssert((colorSurface.GetWidth() == depthStencilSurface.GetWidth()) && 
               (colorSurface.GetHeight() == depthStencilSurface.GetHeight()));
    }
}

size_t RenderSet::GetWidth() const
{
    return m_ColorSurface.GetWidth();
}
size_t RenderSet::GetHeight() const
{
    return m_ColorSurface.GetHeight();
}

void RenderSet::Set(GraphicsDevice& device)
{
    CorrectnessGuard();

    ID3D11RenderTargetView* renderTargetViews =  m_ColorSurface.GetRenderTargetView();
    device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &renderTargetViews, 
                                                          m_DepthStencilSurface.GetDepthStencilView());
}

void RenderSet::CorrectnessGuard()
{
    if (m_ColorSurface.IsValid() && m_DepthStencilSurface.IsValid()) 
    {
        popAssert(m_ColorSurface.GetRenderTargetView());
        popAssert(m_DepthStencilSurface.GetDepthStencilView());

        popAssert(m_ColorSurface.GetWidth() == m_DepthStencilSurface.GetWidth()
               && m_ColorSurface.GetHeight() == m_DepthStencilSurface.GetHeight());
    }
}
void RenderSet::Clear(GraphicsDevice& device, float color[4])
{
    CorrectnessGuard();

    ID3D11DeviceContext* context = device.GetD3D11DeviceContext();
    context->ClearRenderTargetView(m_ColorSurface.GetRenderTargetView(), color);
    context->ClearDepthStencilView(m_DepthStencilSurface.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}