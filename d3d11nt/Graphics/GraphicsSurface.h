#pragma once
#include <d3d11.h>
#include "Graphics/Texture2D.h"
#include "Graphics/GraphicsDevice.h"

enum GraphicsSurfaceType
{
    GraphicsSurfaceType_RENDER_TARGET,
    GraphicsSurfaceType_DEPTH_STENCIL
};

template<class T>
void CreateView(GraphicsDevice& device, Texture2D* texture, T** view);

template<>
void CreateView(GraphicsDevice& device, Texture2D* texture, ID3D11RenderTargetView** view);

template<>
void CreateView(GraphicsDevice& device, Texture2D* texture, ID3D11DepthStencilView** view);

template<class T>
class GraphicsSurface
{
public:
    GraphicsSurface();
    //GraphicsSurface(ID3D11RenderTargetView* renderTargetView);
    //GraphicsSurface(ID3D11DepthStencilView* depthStencilView);
    GraphicsSurface(GraphicsDevice& device, Texture2D* texture);
    ~GraphicsSurface();

    void Resize(GraphicsDevice& device, size_t width, size_t height);
    //void Validate(const GraphicsSurface& surface);

    T* GetView()
    {
        return (T*)m_View;
    }

    size_t GetWidth() const;
    size_t GetHeight() const;
    Texture2D* GetTexture() { return m_Texture; }

    void ReleaseGPUData();
private:
    T* m_View;
    Texture2D* m_Texture;
};

typedef GraphicsSurface<ID3D11RenderTargetView> ColorSurface;
typedef GraphicsSurface<ID3D11DepthStencilView> DepthSurface;