#pragma once
#include <d3d11.h>
#include "Texture2D.h"
#include "GraphicsDevice.h"

enum GraphicsSurfaceType
{
    GraphicsSurfaceType_RENDER_TARGET,
    GraphicsSurfaceType_DEPTH_STENCIL
};

template<class T>
void CreateView(GraphicsDevice& device, Texture2D* texture, T** view);

template<>
void CreateView(GraphicsDevice& device, Texture2D* texture, ID3D11RenderTargetView** view);
/*{
    D3D_HR_OP(device.GetD3D11Device()->CreateRenderTargetView(texture->GetD3D11Texture2D(), nullptr, &view));
}*/

template<>
void CreateView(GraphicsDevice& device, Texture2D* texture, ID3D11DepthStencilView** view);
/*{
    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilView(texture->GetD3D11Texture2D(), nullptr, &view));
}*/

template<class T>
class GraphicsSurface : public D3D11ResourceHolder
{
public:
    GraphicsSurface();
    //GraphicsSurface(ID3D11RenderTargetView* renderTargetView);
    //GraphicsSurface(ID3D11DepthStencilView* depthStencilView);
    GraphicsSurface(GraphicsDevice& device, Texture2D* texture);
    ~GraphicsSurface();

    void Resize(GraphicsDevice& device, size_t width, size_t height);
    //void Validate(const GraphicsSurface& surface); //make sure we can this surr

    T* GetView()
    {
        return (T*)GetDX11Object();
    }

    size_t GetWidth() const;
    size_t GetHeight() const;

    void Release();
private:
    Texture2D* m_Texture;

};

typedef GraphicsSurface<ID3D11RenderTargetView> ColorSurface;
typedef GraphicsSurface<ID3D11DepthStencilView> DepthSurface;