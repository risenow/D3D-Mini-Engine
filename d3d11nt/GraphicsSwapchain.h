#pragma once
#include "Window.h"
#include "GraphicsDevice.h"
#include "GraphicsOptions.h"
#include "GraphicsSurface.h"

class GraphicsSwapChain
{
public:
    GraphicsSwapChain();
    GraphicsSwapChain(const Window& window, GraphicsDevice& device, MultisampleType multisample = MULTISAMPLE_TYPE_NONE);

    void Present();

    ID3D11Texture2D* GetBackBufferTexture();
    GraphicsSurface GetBackBufferSurface();

    IDXGISwapChain* GetDXGISwapChain();

    bool IsValid(const Window& window);

    void Validate(GraphicsDevice& device, const Window& window);

    void InitializeBackBufferSurface(GraphicsDevice& device);
    void SetFullcreenState(bool state);
    //ID3D11RenderTargetView* GetBackBufferRenderTargetView();
private:
    void FillSwapChainDesc(const Window& window, DXGI_SWAP_CHAIN_DESC& desc, const DXGI_SAMPLE_DESC& sampleDesc);

    IDXGISwapChain* m_SwapChain;
    //ID3D11Texture2D* m_BackBufferTexture;
    Texture2D m_BackBufferTexture;
    GraphicsSurface m_BackBufferSurface;
    //ID3D11RenderTargetView* m_BackBufferRenderTargetView;
    DXGI_FORMAT m_Format;

    //size_t m_Width;
    //size_t m_Height;

    bool m_FullsreenState;
};