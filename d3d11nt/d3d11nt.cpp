// d3d11nt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <initguid.h>

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
#include "Graphics/ShadersCollection.h"
#include "Graphics/FrameRateLock.h"
#include "Graphics/SceneGraph.h"
#include "System/HardwareInfo.h"
#include "System/memutils.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include <d3d11.h>
#include <dxgidebug.h>
#include "Graphics/RenderSet.h"
#include "System/IniFile.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

const std::string OPTIONS_INI_FILE_NAME = "options.ini";

void WriteFPSToWindowTitle(Window& window, const FrameMeasurer& frameMeasurer)
{
    std::string newWindowTitle = window.GetTitle();
    newWindowTitle += " FPS(per second counter): " + std::to_string(frameMeasurer.GetFPSBasedOnPerSecondFrameCounter());
    newWindowTitle += " FPS(based on average frametime): " + std::to_string(frameMeasurer.GetFPSBasedOnAverageFrameTime());
    SetWindowText(window.GetWindowHandle(), strtowstr(newWindowTitle).c_str());
}

float DeserializeCameraFOV(const IniFile& ini)
{
	const std::string section = "Graphics";
	const float defaultFOV = 90;
	return ini.GetValue(section, "FOV", defaultFOV);
}

Camera CreateInitialCamera(const IniFile& ini)
{
	const glm::vec3 position = glm::vec3(0.0f, .0f, .8f);
	const glm::vec3 rotation  = glm::vec3(0.0f, 0.0f, 0.0f);
	Camera camera = Camera(position, rotation);
	//camera.SetProjection(glm::perspective(glm::radians(DeserializeCameraFOV(ini)), 1.0f, 0.01f, 1000.0f));
	camera.SetProjection(glm::perspectiveFov(glm::radians(DeserializeCameraFOV(ini)), 512.0f, 512.0f, 0.01f, 1000.0f));
	return camera;
}

int main(int argc, char* argv[])
{
    HardwareInfo::GetInstance().Initialize(); // mb we should "unsignleton" this
    RuntimeLog::GetInstance().SetMode(RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine);

    AutoSaveIniFile iniFile(OPTIONS_INI_FILE_NAME, IniFile::LoadingFlags_FileMayNotExist);
    CommandLineArgs commandLineArgs(iniFile);
    FrameMeasurer frameMeasurer;
	MouseKeyboardCameraController mouseKeyboardCameraController(CreateInitialCamera(iniFile), iniFile);
    GraphicsOptions options(iniFile);
    Window window(iniFile);
    DisplayAdaptersList displayAdaptersList;
    D3D11DeviceCreationFlags deviceCreationFlags(commandLineArgs); 
    GraphicsDevice device(deviceCreationFlags, FEATURE_LEVEL_ONLY_D3D11, nullptr);
    GraphicsSwapChain swapchain(device, window, options.GetMultisampleType());

	ShaderID testVSBATCHID = GetShaderID(L"Test/vs.vs", { GraphicsShaderMacro("BATCH", "1") });
	ShaderID testPSBATCHID = GetShaderID(L"Test/ps.ps", { GraphicsShaderMacro("BATCH", "1") });

	ShadersCollection shadersCollection(device);
	shadersCollection.AddShader<GraphicsVertexShader>(L"Test/vs.vs", { { GraphicsShaderMacro("BATCH", "1") } });
	shadersCollection.AddShader<GraphicsPixelShader> (L"Test/ps.ps", { { GraphicsShaderMacro("BATCH", "1") } });
	shadersCollection.ExecuteShadersCompilation(device);

	SceneGraph sceneGraph(device, shadersCollection, "Data/scene.xml");

	FrameRateLock frameRateLock(100);

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

   /* const std::string POSITION_PROPERTY_NAME = "POSITION";
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
	consts.coef = 0.4f;
	consts.viewProjection = mouseKeyboardCameraController.GetCamera().GetViewProjectionMatrix();
	GraphicsConstantsBuffer<constsTest> constantsBuffer(device);
	//consts.

	GraphicsVertexShader vertexShader(device, "vs.vs");
	GraphicsPixelShader pixelShader(device, "ps.ps");

	GraphicsInputLayout inputLayout(device, vertexFormat, vertexShader);*/

	//**DRAW FRAMEWORK**//

    float clearColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

    while (!window.IsClosed())
    {
		//std::lock_guard<std::mutex> lockGuard(shadersCollection.m_ShadersMutex);
		//while (!window.IsFocused())
        //    std::this_thread::sleep_for(std::chrono::milliseconds(1));

		frameRateLock.OnFrameBegin();
        frameMeasurer.BeginMeasurement();

		//shadersCollection.ExecuteShadersCompilation(device);

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

		sceneGraph.GetOrdinaryGraphicsObjectManager()->Render(device, shadersCollection, mouseKeyboardCameraController.GetCamera());

        if (swapchain.IsValid(window, options.GetMultisampleType())) //if swapchain is not valid discard the frame //POSSIBLE RACE CONDITION?
            swapchain.Present();

		frameRateLock.OnFrameEnd();
		frameMeasurer.EndMeasurement();

        WriteFPSToWindowTitle(window, frameMeasurer);

		if (window.IsFocused())
		mouseKeyboardCameraController.Update(window);
    }

    if (device.GetD3D11DeviceContext())
        device.GetD3D11DeviceContext()->ClearState();

    DEBUG_ONLY(device.ReportAllLiveObjects());

    window.SerializeToIni(iniFile);
    options.SerializeToIni(iniFile);
    return 0;
}

