// d3d11nt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "MemoryManager.h"
#include "Window.h"
#include "CommandLineArgs.h"
#include "GraphicsDevice.h"
#include "GraphicsOptions.h"
#include "GraphicsSwapchain.h"
#include "GraphicsViewport.h"
#include "DisplayAdaptersList.h"
#include "memutils.h"
#include <d3d11.h>
#include "RenderSet.h"
#include "IniFile.h"
#include <iostream>

int main(int argc, char* argv[])
{
    MemoryManager::Initialize(64 MB);

    CommandLineArgs commandLineArgs(argv, argc);
    IniFile iniFile("options.ini");
    GraphicsOptions options(iniFile);
    WindowAttributes windowAttributes(iniFile);
    Window window(windowAttributes);
    DisplayAdaptersList displayAdaptersList;
    D3D11DeviceCreationFlags deviceCreationFlags(true, false);
    GraphicsDevice device(deviceCreationFlags, FEATURE_LEVEL_ONLY_D3D11, nullptr);
    GraphicsSwapChain swapchain(window, device, options.GetMultisampleType());

    GraphicsSurface backBufferSurface = swapchain.GetBackBufferSurface();
    Texture2D depthStencilTexture = Texture2DHelper::CreateCommonTexture(device,
                                                                         backBufferSurface.GetWidth(),
                                                                         backBufferSurface.GetHeight(),
                                                                         1,
                                                                         DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                                         options.GetMultisampleType(),
                                                                         D3D11_BIND_DEPTH_STENCIL);
    GraphicsSurface depthStencilSurface(device, depthStencilTexture, GRAPHICS_SURFACE_TYPE_DEPTH_STENCIL);
    RenderSet finalRenderSet(backBufferSurface, depthStencilSurface);

    GraphicsViewport viewport(finalRenderSet);

    float clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

    while (!window.IsClosed())
    {
        if (!swapchain.IsValid(window))
        {
            swapchain.Validate(device, window);
            backBufferSurface = swapchain.GetBackBufferSurface();
            depthStencilSurface.Release();
            depthStencilTexture.Resize(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight());
            finalRenderSet = RenderSet(backBufferSurface, GraphicsSurface(device, depthStencilTexture, GRAPHICS_SURFACE_TYPE_DEPTH_STENCIL));
        }
        //TO INCAPSULATE IN RENDERPASS class
        //finalRenderSet.Set(device);
        //viewport.Set(device);
        finalRenderSet.Clear(device, clearColor);

        if (swapchain.IsValid(window)) //if swapchain in not valid discard the frame //POSSIBLE RACE CONDITION?
            swapchain.Present();
    }

    return 0;
}

