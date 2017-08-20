// d3d11nt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <initguid.h>

#include "Data/Shaders/Test/constants.h"

#include "System/MemoryManager.h"
#include "System/Window.h"
#include "System/CommandLineArgs.h"
#include "System/Camera.h"
#include "System/MouseKeyboardCameraController.h"
#include "Graphics/FrameMeasurer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsOptions.h"
#include "Graphics/GraphicsSwapchain.h"
#include "Graphics/GraphicsViewport.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/GraphicsInputLayout.h"
#include "Graphics/DisplayAdaptersList.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/VertexData.h"
#include "System/HardwareInfo.h"
#include "System/memutils.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
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

Camera CreateInitialCamera()
{
	const glm::vec3 position = glm::vec3(0.0f, .0f, .8f);
	const glm::vec3 rotation  = glm::vec3(0.0f, 0.0f, 0.0f);
	Camera camera = Camera(position, rotation);
	camera.SetProjection(glm::perspective(1.48f, 1.0f, 0.01f, 1000.0f));
	return camera;
}

int main(int argc, char* argv[])
{
    HardwareInfo::GetInstance().Initialize();
    RuntimeLog::GetInstance().SetMode(RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine);

    AutoSaveIniFile iniFile(OPTIONS_INI_FILE_NAME, IniFile::LoadingFlags_FileMayNotExist);
    CommandLineArgs commandLineArgs(iniFile);
    FrameMeasurer frameMeasurer;
	MouseKeyboardCameraController mouseKeyboardCameraController(CreateInitialCamera(), iniFile);
    GraphicsOptions options(iniFile);
    Window window(iniFile);
    DisplayAdaptersList displayAdaptersList;
    D3D11DeviceCreationFlags deviceCreationFlags(commandLineArgs); 
    GraphicsDevice device(deviceCreationFlags, FEATURE_LEVEL_ONLY_D3D11, nullptr);
    GraphicsSwapChain swapchain(device, window, options.GetMultisampleType());

	window.AddCommand(SetCursorVisibilityCommand(false));

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

	//**DRAW FRAMEWORK**//

    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::vec4 PositionType;

    VertexFormat vertexFormat;
	vertexFormat.AddVertexProperty(CreateVertexPropertyPrototype<PositionType>(POSITION_PROPERTY_NAME));

	const count_t VERTEX_COUNT = 3;
    VertexData vertexData(vertexFormat, VERTEX_COUNT);
    VertexProperty positionProperty = vertexData.GetVertexPropertyByName(POSITION_PROPERTY_NAME);
    (*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(0, positionProperty)) = (PositionType(-0.5f, -0.5f, 0.0f, 1.0f));
    (*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(1, positionProperty)) = (PositionType(-0.5f, 0.5f, 0.0f, 1.0f));
	(*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(2, positionProperty)) = (PositionType(0.5f, -0.5f, 0.0f, 1.0f));

	VertexBuffer vertexBuffer(device, vertexData);

	constsTest consts;
	consts.coef = 0.8f;
	consts.viewProjection = mouseKeyboardCameraController.GetCamera().GetViewProjectionMatrix();
	GraphicsConstantsBuffer<constsTest> constantsBuffer(device);
	//consts.

	GraphicsVertexShader vertexShader(device, "vs.vs");
	GraphicsPixelShader pixelShader(device, "ps.ps");

	GraphicsInputLayout inputLayout(device, vertexFormat, vertexShader);

	//**DRAW FRAMEWORK**//

    float clearColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

	int s = ShowCursor(false);

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

		consts.viewProjection = mouseKeyboardCameraController.GetCamera().GetViewProjectionMatrix();
		constantsBuffer.Update(device, consts);

		vertexShader.Bind(device);
		pixelShader.Bind(device);

		constantsBuffer.Bind(device, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel);

		inputLayout.Bind(device);
		vertexBuffer.Bind(device);

		device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		device.GetD3D11DeviceContext()->Draw(VERTEX_COUNT, 0);

        if (swapchain.IsValid(window, options.GetMultisampleType())) //if swapchain is not valid discard the frame //POSSIBLE RACE CONDITION?
            swapchain.Present();

        frameMeasurer.EndMeasurement();
        WriteFPSToWindowTitle(window, frameMeasurer);

		mouseKeyboardCameraController.Update(window);
    }

    if (device.GetD3D11DeviceContext())
        device.GetD3D11DeviceContext()->ClearState();

    DEBUG_ONLY(device.ReportAllLiveObjects());

    window.SerializeToIni(iniFile);
    options.SerializeToIni(iniFile);
    return 0;
}

