#pragma once
#include "System/Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture2D.h"
#include "Graphics/GraphicsSurface.h"
#include "Graphics/GraphicsOptions.h"
#include <memory>
#include <d3d11.h>

class RenderSet
{
public:
    RenderSet();
    RenderSet(GraphicsDevice& device, size_t w, size_t h, MultisampleType ms, const std::vector<DXGI_FORMAT>& formats);
    RenderSet(const std::vector<ColorSurface>& colorSurface, const DepthSurface& depthStencilSurface);
    //RenderSet(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, bool useDepthStencil);
    RenderSet(size_t width, size_t height, DXGI_FORMAT colorSurfaceFormat, DXGI_FORMAT depthSurfaceFormat, bool useDepthStencil = false);

    size_t GetWidth() const;
    size_t GetHeight() const;

    ID3D11RenderTargetView* GetRenderTargetView() const;
    ID3D11DepthStencilView* GetDepthStencilView() const;

    void Set(GraphicsDevice& device);
    void Clear(GraphicsDevice& device, float color[4]);

    Texture2D& GetColorTexture(size_t idx)
    {
        assert(idx < m_RenderTargetTextures.size());
        assert(idx < m_ColorSurfaces.size());
        return m_RenderTargetTextures[idx];
    }
private:
    void CorrectnessGuard();

    std::vector<ColorSurface> m_ColorSurfaces;
    DepthSurface m_DepthStencilSurface;
    std::vector<Texture2D> m_RenderTargetTextures;
    Texture2D m_DepthStencilTexture;
};
