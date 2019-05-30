#pragma once
#include "System/Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsOptions.h"
#include "Graphics/GraphicsSurface.h"

class GraphicsSwapChain
{
public:
    GraphicsSwapChain();
    GraphicsSwapChain(GraphicsDevice& device, const Window& window, MultisampleType multisample = MULTISAMPLE_TYPE_NONE);

    ~GraphicsSwapChain();

    void Present();

    ID3D11Texture2D* GetBackBufferTexture();
    ColorSurface GetBackBufferSurface();

    IDXGISwapChain* GetDXGISwapChain();

    bool IsValid(const Window& window, MultisampleType currentlySelectedMultisampleType);

    void Validate(GraphicsDevice& device, const Window& window, MultisampleType currentlySelectedMultisampleType);

    void InitializeBackBufferSurface(GraphicsDevice& device);
    void SetFullcreenState(bool state);
    //ID3D11RenderTargetView* GetBackBufferRenderTargetView();
private:
    void FillSwapChainDesc(const Window& window, DXGI_SWAP_CHAIN_DESC& desc, const DXGI_SAMPLE_DESC& sampleDesc);
    void UpdateDisplayModesList();
    void UpdateDisplayModesList(IDXGIOutput* output);

    IDXGISwapChain* m_SwapChain;
    //ID3D11Texture2D* m_BackBufferTexture;
    Texture2D m_BackBufferTexture;
    ColorSurface m_BackBufferSurface;
    //ID3D11RenderTargetView* m_BackBufferRenderTargetView;
    DXGI_FORMAT m_Format;
    STLVector<DXGI_MODE_DESC> m_DisplayModeDescsList;
    //size_t m_Width;
    //size_t m_Height;

    bool m_FullsreenState;
};