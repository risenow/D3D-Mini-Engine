#include "stdafx.h"
#include "GraphicsSwapchain.h"
#include "logicsafety.h"
#include "multisampleutils.h"
#include <dxgi.h>
#include <thread>

GraphicsSwapChain::GraphicsSwapChain() {}
GraphicsSwapChain::GraphicsSwapChain(const Window& window, GraphicsDevice& device, MultisampleType multisample)
                                   : m_Format(DXGI_FORMAT_R8G8B8A8_UNORM), m_SwapChain(nullptr), m_FullsreenState(false)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    FillSwapChainDesc(window, swapChainDesc, GetSampleDesc(device, m_Format, multisample));

    IDXGIDevice* giDev;
    D3D_HR_OP(device.GetD3D11Device()->QueryInterface(__uuidof(IDXGIDevice), (void**)&giDev));
    IDXGIAdapter* giAdapter;
    D3D_HR_OP(giDev->GetAdapter(&giAdapter));
    IDXGIFactory* giFactory;
    D3D_HR_OP(giAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&giFactory));

    D3D_HR_OP(giFactory->CreateSwapChain(device.GetD3D11Device(), &swapChainDesc, &m_SwapChain));

    giFactory->MakeWindowAssociation(window.GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);

    giDev->Release();
    giAdapter->Release();
    giFactory->Release();

    InitializeBackBufferSurface(device);
}

void GraphicsSwapChain::InitializeBackBufferSurface(GraphicsDevice& device)
{
    ID3D11Texture2D* backBuffer;
    D3D_HR_OP(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&backBuffer)));
    m_BackBufferTexture = Texture2D(backBuffer);
    m_BackBufferSurface = GraphicsSurface(device, &m_BackBufferTexture,
        GRAPHICS_SURFACE_TYPE_RENDER_TARGET);
}

IDXGISwapChain* GraphicsSwapChain::GetDXGISwapChain()
{
    return m_SwapChain;
}

void GraphicsSwapChain::Present()
{
    D3D_HR_OP(m_SwapChain->Present(0, 0));
}

bool GraphicsSwapChain::IsValid(const Window& window)
{
    if ((window.GetWindowMode() == WINDOW_MODE_FULLSCREEN) != m_FullsreenState)
        return false;
    DXGI_SWAP_CHAIN_DESC swapchainDesc;
    D3D_HR_OP(m_SwapChain->GetDesc(&swapchainDesc));
    DXGI_MODE_DESC bufferDesc = swapchainDesc.BufferDesc;
    if (window.GetWidth() != bufferDesc.Width || window.GetHeight() != bufferDesc.Height)
        return false;
    return true;
}

void GraphicsSwapChain::Validate(GraphicsDevice& device, const Window& window)
{
    if (!IsValid(window))
    {
        m_BackBufferSurface.Release();
        m_BackBufferTexture.Release();

        bool fullscreen = window.GetWindowMode() == WINDOW_MODE_FULLSCREEN;
        m_FullsreenState = false;
        if (fullscreen)
        {
            IDXGIOutput* output;
            D3D_HR_OP(m_SwapChain->GetContainingOutput(&output));
            unsigned int modesNum = 0;
            output->GetDisplayModeList(m_Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH, &modesNum, nullptr);
            assert(modesNum);
            STLVector<DXGI_MODE_DESC> modeDescs(modesNum);
            output->GetDisplayModeList(m_Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH, &modesNum, modeDescs.data());

            m_SwapChain->SetFullscreenState(true, output);
            D3D_HR_OP(m_SwapChain->ResizeTarget(&modeDescs[modesNum - 1]));
            m_FullsreenState = true;
        }
        if (!m_FullsreenState)
            m_SwapChain->SetFullscreenState(false, nullptr);

        D3D_HR_OP(m_SwapChain->ResizeBuffers(1, window.GetWidth(), window.GetHeight(), m_Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
        InitializeBackBufferSurface(device);
    }
}

//to change return type to Texture2D
ID3D11Texture2D* GraphicsSwapChain::GetBackBufferTexture()
{
    return nullptr;
}
GraphicsSurface* GraphicsSwapChain::GetBackBufferSurface()
{
    return &m_BackBufferSurface;
}

void GraphicsSwapChain::FillSwapChainDesc(const Window& window, DXGI_SWAP_CHAIN_DESC& desc, 
                                          const DXGI_SAMPLE_DESC& sampleDesc)
{
    desc.BufferDesc.Width = 0;
    desc.BufferDesc.Height = 0;
    desc.BufferDesc.RefreshRate.Numerator = 0;
    desc.BufferDesc.RefreshRate.Denominator = 0;
    desc.BufferDesc.Format = m_Format;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    desc.SampleDesc = sampleDesc;

    desc.BufferCount = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    desc.OutputWindow = window.GetWindowHandle();
    desc.Windowed = (window.GetWindowMode() == WINDOW_MODE_WINDOWED);
}

void GraphicsSwapChain::SetFullcreenState(bool state)
{
    if (m_FullsreenState != state)
    {
        m_FullsreenState = state;
        //m_FullscreenStateChanged = true;
    }
}
