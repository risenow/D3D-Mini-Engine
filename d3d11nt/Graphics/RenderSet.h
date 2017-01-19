#pragma once
#include "System/Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture2D.h"
#include "Graphics/GraphicsSurface.h"
#include <d3d11.h>

class RenderSet
{
public:
    RenderSet();
    RenderSet(ColorSurface* colorSurface, DepthSurface* depthStencilSurface);
    //RenderSet(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, bool useDepthStencil);
    RenderSet(size_t width, size_t height, DXGI_FORMAT colorSurfaceFormat, DXGI_FORMAT depthSurfaceFormat, bool useDepthStencil = false);

    size_t GetWidth() const;
    size_t GetHeight() const;

    ID3D11RenderTargetView* GetRenderTargetView() const;
    ID3D11DepthStencilView* GetDepthStencilView() const;

    void Set(GraphicsDevice& device);
    void Clear(GraphicsDevice& device, float color[4]);
private:
    void CorrectnessGuard();
    //ID3D11Texture2D* m_RenderTargetTexture;
    //ID3D11Texture2D* m_DepthStencilTexture;

    ColorSurface* m_ColorSurface;
    DepthSurface* m_DepthStencilSurface;
    Texture2D m_RenderTargetTexture;
    Texture2D m_DepthStencilTexture;

    /*ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11DepthStencilView* m_DepthStencilView;*/
};
