#pragma once
#include <d3d11.h>
#include "Texture2D.h"
#include "GraphicsDevice.h"

enum GraphicsSurfaceType
{
    GRAPHICS_SURFACE_TYPE_RENDER_TARGET,
    GRAPHICS_SURFACE_TYPE_DEPTH_STENCIL
};

class GraphicsSurface
{
public:
    GraphicsSurface();
    //GraphicsSurface(ID3D11RenderTargetView* renderTargetView);
    //GraphicsSurface(ID3D11DepthStencilView* depthStencilView);
    GraphicsSurface(GraphicsDevice& device, Texture2D* texture, GraphicsSurfaceType type);
    ~GraphicsSurface();

    void Resize(GraphicsDevice& device, size_t width, size_t height);
    //void Validate(const GraphicsSurface& surface); //make sure we can this surr

    ID3D11RenderTargetView* GetRenderTargetView() const;
    ID3D11DepthStencilView* GetDepthStencilView() const;

    size_t GetWidth() const;
    size_t GetHeight() const;

    bool IsValid() const;

    void Release();
private:
    Texture2D* m_Texture;
    ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11DepthStencilView* m_DepthStencilView;
};