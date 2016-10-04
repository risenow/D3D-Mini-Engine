#include "stdafx.h"
#include "RenderSet.h"
#include "logicsafety.h"

RenderSet::RenderSet() {}

RenderSet::RenderSet(ColorSurface* colorSurface, DepthSurface* depthStencilSurface) 
                   : m_ColorSurface(colorSurface), m_DepthStencilSurface(depthStencilSurface) 
{
    CorrectnessGuard();
}

size_t RenderSet::GetWidth() const
{
    return m_ColorSurface->GetWidth();
}
size_t RenderSet::GetHeight() const
{
    return m_ColorSurface->GetHeight();
}

void RenderSet::Set(GraphicsDevice& device)
{
    CorrectnessGuard();

    ID3D11RenderTargetView* renderTargetViews = m_ColorSurface->GetView();
    device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &renderTargetViews, 
                                                          m_DepthStencilSurface->GetView());
}

void RenderSet::CorrectnessGuard()
{
    popAssert(m_ColorSurface->GetView() || m_DepthStencilSurface->GetView());
    if (m_ColorSurface && m_DepthStencilSurface)
    {
        popAssert(m_ColorSurface->GetView() && m_DepthStencilSurface->GetView());
        popAssert(m_ColorSurface->GetWidth() == m_DepthStencilSurface->GetWidth() &&
                  m_ColorSurface->GetHeight() == m_DepthStencilSurface->GetHeight());
    }
}
void RenderSet::Clear(GraphicsDevice& device, float color[4])
{
    CorrectnessGuard();

    ID3D11DeviceContext* context = device.GetD3D11DeviceContext();
    if (m_ColorSurface)
        context->ClearRenderTargetView(m_ColorSurface->GetView(), color);
    if (m_DepthStencilSurface)
        context->ClearDepthStencilView(m_DepthStencilSurface->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}