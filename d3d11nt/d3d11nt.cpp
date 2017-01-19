// d3d11nt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <initguid.h>

#include "System/MemoryManager.h"
#include "System/Window.h"
#include "System/CommandLineArgs.h"
#include "System/Camera.h"
#include "Graphics/FrameMeasurer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsOptions.h"
#include "Graphics/GraphicsSwapchain.h"
#include "Graphics/GraphicsViewport.h"
#include "Graphics/DisplayAdaptersList.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/VertexData.h"
#include "System/HardwareInfo.h"
#include "System/memutils.h"
#include <d3d11.h>
#include <dxgidebug.h>
#include "Graphics/RenderSet.h"
#include "System/IniFile.h"
#include <iostream>
#include <thread>
#include <chrono>

const std::string OPTIONS_INI_FILE_NAME = "options.ini";

void WriteFPSToWindowTitle(Window& window, const FrameMeasurer& frameMeasurer)
{
    std::string newWindowTitle = window.GetTitle();
    newWindowTitle += " FPS(per second counter): " + std::to_string(frameMeasurer.GetFPSBasedOnPerSecondFrameCounter());
    newWindowTitle += " FPS(based on average frametime): " + std::to_string(frameMeasurer.GetFPSBasedOnAverageFrameTime());
    SetWindowText(window.GetWindowHandle(), strtowstr(newWindowTitle).c_str());
}

int main(int argc, char* argv[])
{
    HardwareInfo::GetInstance().Initialize();
    RuntimeLog::GetInstance().SetMode(RuntimeLogMode_FileOutput | RuntimeLogMode_ConsoleOutput | RuntimeLogMode_WriteTime);

    AutoSaveIniFile iniFile(OPTIONS_INI_FILE_NAME, IniFile::LoadingFlags_FileMayNotExist);
    CommandLineArgs commandLineArgs(iniFile);
    FrameMeasurer frameMeasurer;
    Camera camera;
    GraphicsOptions options(iniFile);
    Window window(iniFile);
    DisplayAdaptersList displayAdaptersList;
    D3D11DeviceCreationFlags deviceCreationFlags(commandLineArgs); 
    GraphicsDevice device(deviceCreationFlags, FEATURE_LEVEL_ONLY_D3D11, nullptr);

    GraphicsSwapChain swapchain(device, window, options.GetMultisampleType());

    ColorSurface* backBufferSurface = swapchain.GetBackBufferSurface();

    Texture2D depthStencilTexture = Texture2DHelper::CreateCommonTexture(device,
                                                                         backBufferSurface->GetWidth(),
                                                                         backBufferSurface->GetHeight(),
                                                                         1,
                                                                         DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                                         options.GetMultisampleType(),
                                                                         D3D11_BIND_DEPTH_STENCIL);
    DEBUG_ONLY(depthStencilTexture.SetDebugName("Depth Stencil"));

    DepthSurface depthStencilSurface(device, &depthStencilTexture);
    RenderSet finalRenderSet(backBufferSurface, &depthStencilSurface);

    GraphicsViewport viewport(finalRenderSet);

    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::vec4 PositionType;

    VertexFormat vertexFormat;
    vertexFormat.AddVertexProperty(POSITION_PROPERTY_NAME, sizeof(PositionType));

    VertexData vertexData(vertexFormat, 2);
    VertexProperty positionProperty = vertexData.GetVertexPropertyByName(POSITION_PROPERTY_NAME);
    (*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(0, positionProperty)) = (PositionType(0.0f, 0.0f, 0.0f, 1.0f));
    (*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(1, positionProperty)) = (PositionType(0.0f, 0.0f, 0.0f, 1.0f));

    VertexBuffer vertexBuffer(device, vertexData);

    float clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

    while (!window.IsClosed())
    {
        while (!window.IsFocused())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        frameMeasurer.BeginMeasurement();

        viewport = GraphicsViewport(window);
        if (!swapchain.IsValid(window, options.GetMultisampleType()))
        {
            swapchain.Validate(device, window, options.GetMultisampleType());
            depthStencilSurface.Resize(device, backBufferSurface->GetWidth(), backBufferSurface->GetHeight());
        }
        //TO INCAPSULATE IN RENDERPASS class
        finalRenderSet.Set(device);
        viewport.Set(device);
        finalRenderSet.Clear(device, clearColor);

        if (swapchain.IsValid(window, options.GetMultisampleType())) //if swapchain in not valid discard the frame //POSSIBLE RACE CONDITION?
            swapchain.Present();

        frameMeasurer.EndMeasurement();

        WriteFPSToWindowTitle(window, frameMeasurer);
    }

    if (device.GetD3D11DeviceContext())
        device.GetD3D11DeviceContext()->ClearState();

    DEBUG_ONLY(device.ReportAllLiveObjects());

    window.SerializeToIni(iniFile);
    options.SerializeToIni(iniFile);
    return 0;
}

