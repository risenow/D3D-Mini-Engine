#include "stdafx.h"
#include "Graphics/GraphicsSurface.h"
#include "System/logicsafety.h"
#include "Graphics/dxlogicsafety.h"


template<class T>
void CreateView(GraphicsDevice& device, Texture2D* texture, T** view) {}

template<>
void CreateView(GraphicsDevice& device, Texture2D* texture, ID3D11RenderTargetView** view)
{
    D3D_HR_OP(device.GetD3D11Device()->CreateRenderTargetView(texture->GetD3D11Texture2D(), nullptr, view));
}

template<>
void CreateView(GraphicsDevice& device, Texture2D* texture, ID3D11DepthStencilView** view)
{
    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilView(texture->GetD3D11Texture2D(), nullptr, view));
}


template<class T>
GraphicsSurface<T>::GraphicsSurface() {}

template<class T>
GraphicsSurface<T>::GraphicsSurface(GraphicsDevice& device, Texture2D* texture)
                                : m_Texture(texture)
{
    CreateView(device, texture, (T**)(&GetDX11ObjectReference()));
}
template<class T>
GraphicsSurface<T>::~GraphicsSurface()
{
}

template<class T>
void GraphicsSurface<T>::Resize(GraphicsDevice& device, size_t width, size_t height)
{
    popAssert(m_Texture);
    popAssert(GetDX11Object());

    if (m_Texture->GetWidth() != width || m_Texture->GetWidth() != height)
    {
        ReleaseDX11Object();

        m_Texture->Resize(device, width, height);

        CreateView(device, m_Texture, (T**)(&GetDX11ObjectReference()));
    }
}

template<class T>
size_t GraphicsSurface<T>::GetWidth() const
{
    return m_Texture->GetWidth();
}
template<class T>
size_t GraphicsSurface<T>::GetHeight() const
{
    return m_Texture->GetHeight();
}

template<class T>
void GraphicsSurface<T>::Release()
{
    ReleaseDX11Object();
}

template class GraphicsSurface<ID3D11RenderTargetView>;
template class GraphicsSurface<ID3D11DepthStencilView>;