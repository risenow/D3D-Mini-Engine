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
#include "Graphics/GraphicsTextureCollection.h"
#include "Graphics/FrameRateLock.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/GraphicsCubemapTexture.h"
#include "System/HardwareInfo.h"
#include "System/memutils.h"
#define cimg_use_jpeg
#define cimg_use_png
//#include "CImg.h"
//#include "IL/il.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtx/common.hpp"
#include <d3d11.h>
//#include <D3DX11.h>
//#include "D3D11X.h"//D3DX11.H
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
	const glm::vec3 position = glm::vec3(0.0f, .0f, .0f);
	const glm::vec3 rotation  = glm::vec3(0.0f, 0.0f, 0.0f);
	Camera camera = Camera(position, rotation);
    float deg = DeserializeCameraFOV(ini);
	//camera.SetProjection(glm::perspective(glm::radians(DeserializeCameraFOV(ini)), 1.0f, 0.01f, 1000.0f));
	camera.SetProjection((glm::perspectiveFov(glm::radians(45.0f), 512.0f, 512.0f, 1.0f, 1000.0f)));

    glm::vec4 tposition = glm::vec4(0.5f, .5f, 1.5f,1.0);
    //tposition = // tposition * camera.GetProjectionMatrix();

	return camera;
}

int main(int argc, char* argv[])
{
    CoInitialize(nullptr);
    HardwareInfo::GetInstance().Initialize(); // mb we should "unsignleton" this
    RuntimeLog::GetInstance().SetMode(RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine);

    //int a;
    //std::cin >> a;//renderdoc pause

    AutoSaveIniFile iniFile(OPTIONS_INI_FILE_NAME, IniFile::LoadingFlags_FileMayNotExist);
    CommandLineArgs commandLineArgs(iniFile);
    FrameMeasurer frameMeasurer;
	MouseKeyboardCameraController mouseKeyboardCameraController(CreateInitialCamera(iniFile), iniFile);
    GraphicsOptions options(iniFile);
    Window window(iniFile);
    DisplayAdaptersList displayAdaptersList;
    D3D11DeviceCreationFlags deviceCreationFlags(commandLineArgs); 

    int a;
    std::cin >> a;

    GraphicsDevice device(deviceCreationFlags, FEATURE_LEVEL_ONLY_D3D11, nullptr);
    GraphicsSwapChain swapchain(device, window, options.GetMultisampleType());

	ShaderID testVSBATCHID = GetShaderID(L"Test/vs.hlsl", { GraphicsShaderMacro("BATCH", "1") });
	ShaderID testPSBATCHID = GetShaderID(L"Test/ps.hlsl", { GraphicsShaderMacro("BATCH", "1") });

	ShadersCollection shadersCollection(device);
	shadersCollection.AddShader<GraphicsVertexShader>(L"Test/vs.hlsl", { { GraphicsShaderMacro("BATCH", "1") } });
	shadersCollection.AddShader<GraphicsPixelShader> (L"Test/ps.hlsl", { { GraphicsShaderMacro("BATCH", "1") } });
    shadersCollection.AddShader<GraphicsComputeShader>(L"Test/cs.hlsl", { { GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("HORIZONTAL", "0") } });
    shadersCollection.AddShader<GraphicsComputeShader>(L"Test/cs.hlsl", { { GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("VERTICAL", "0") } });
    shadersCollection.AddShader<GraphicsVertexShader>(L"Test/tessvs.hlsl", { { GraphicsShaderMacro("BATCH", "1") } });
    shadersCollection.AddShader<GraphicsHullShader>(L"Test/tesshs.hlsl", { { GraphicsShaderMacro("BATCH", "1") } });
    shadersCollection.AddShader<GraphicsDomainShader>(L"Test/tessds.hlsl", { { GraphicsShaderMacro("BATCH", "1") } });
    shadersCollection.AddShader<GraphicsPixelShader>(L"Test/tessps.hlsl", { { GraphicsShaderMacro("BATCH", "1") } });
	shadersCollection.ExecuteShadersCompilation(device);

    GraphicsTextureCollection textureCollection;
    textureCollection.Add(device, "displ.png");
    textureCollection.Add(device, "cubemap.dds", DXGI_FORMAT_R8G8B8A8_UNORM);
    //textureCollection.Add(device, "spherem1.jpg", DXGI_FORMAT_BC1_UNORM);
    //device.GetD3D11DeviceContext()->Dispatch()

    //int a;
    //std::cin >> a;//renderdoc pause

	SceneGraph sceneGraph(device, textureCollection, shadersCollection, "Data/scene.xml");

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

    swapchain.SetFullcreenState(false);
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

    D3D11_RASTERIZER_DESC rastState;
    rastState.AntialiasedLineEnable = true;
    rastState.CullMode = D3D11_CULL_NONE;
    rastState.FillMode = D3D11_FILL_SOLID;
    rastState.FrontCounterClockwise = FALSE;
    rastState.DepthBias = 0;
    rastState.SlopeScaledDepthBias = 0.0f;
    rastState.DepthBiasClamp = 0.0f;
    rastState.DepthClipEnable = FALSE;
    rastState.ScissorEnable = FALSE;
    rastState.MultisampleEnable = FALSE;

    ID3D11RasterizerState* d3dRastState;

    D3D_HR_OP(device.GetD3D11Device()->CreateRasterizerState(&rastState, &d3dRastState));
    device.GetD3D11DeviceContext()->RSSetState(d3dRastState);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* m_DepthStencilState;
    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState));
    device.GetD3D11DeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 0);

    //std::ifstream shaderFile("Data/Shaders/Test/ps.hlsl");
    //std::string content = std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
    //shaderFile.close();
    //GraphicsShaderMacro passMacro = GraphicsShaderMacro("HORIZONTAL", "0");
    GraphicsComputeShader horBlurComputeShader = shadersCollection.GetShader<GraphicsComputeShader>(L"Test/cs.hlsl", { GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("HORIZONTAL", "0") });//GraphicsComputeShader(device, "Data/Shaders/Test/cs.hlsl", {passMacro});//GraphicsPixelShader::CreateFromTextContent(device, strtowstr(""), content);//GraphicsPixelShader(device, "Test/ps.hlsl");

    GraphicsComputeShader verBlurComputeShader = shadersCollection.GetShader<GraphicsComputeShader>(L"Test/cs.hlsl", { GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("VERTICAL", "0") });//GraphicsComputeShader(device, "Data/Shaders/Test/cs.hlsl", { passMacro });


    GraphicsHullShader hullShader = shadersCollection.GetShader<GraphicsHullShader>(L"Test/tesshs.hlsl", { GraphicsShaderMacro("BATCH", "1") });
    GraphicsDomainShader domainShader = shadersCollection.GetShader<GraphicsDomainShader>(L"Test/tessds.hlsl", { GraphicsShaderMacro("BATCH", "1") });

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    ID3D11UnorderedAccessView* uav;

    device.GetD3D11Device()->CreateUnorderedAccessView(swapchain.GetBackBufferSurface()->GetTexture()->GetD3D11Texture2D(), &uavDesc, &uav);

//    ilInit();

  //  ILuint image;
   // ilGenImages(1, &image);
   // ilBindImage(image);

   // ILboolean b = ilLoadImage(L"displ.jpg");

    //ILenum error = ilGetError();

    //if (error == 1291)
    //    LOG(std::string("IL NOT FING WORK"));

    //ILuint width, height, bytesPerPixel;
    //width = ilGetInteger(IL_IMAGE_WIDTH);
    //height = ilGetInteger(IL_IMAGE_HEIGHT);
    //bytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    //using namespace cimg_library;

    //const CImg<unsigned char> img = CImg<>("displ.png");
    //const unsigned char* data = img.data();
    //unsigned char d1 = *(data+600);
    //unsigned char d2 = *(data+601);
    //unsigned char d3 = *(data+602);
    //unsigned char d4 = *(data+ 603);

    //DXGI_SAMPLE_DESC desc;
    //desc.Count = 1;
    //desc.Quality = 0;
    //D3D11_SUBRESOURCE_DATA subr;
    //subr.pSysMem = data;
    //subr.SysMemPitch = img.width() * sizeof(char) * 4;
    //subr.SysMemSlicePitch = img.width() * img.height() * sizeof(char) * 4;
    //Texture2D t = Texture2D(device, img.width(), img.height(), 1, 1, DXGI_FORMAT_R8G8B8A8_UINT, desc, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, (D3D11_SUBRESOURCE_DATA*)&subr);

    //device.GetD3D11DeviceContext()->OMSetDepthStencilState()
    GraphicsCubemapTexture cubemap(device, "cubemap.dds");
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
        //ID3D11RenderTargetView* renderTargetViews = backBufferSurface->GetView();
        //device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &renderTargetViews,
        //    nullptr);

        viewport.Set(device);

        device.GetD3D11DeviceContext()->RSSetState(d3dRastState);
        device.GetD3D11DeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 0);

        finalRenderSet.Clear(device, clearColor);

		sceneGraph.GetOrdinaryGraphicsObjectManager()->Render(device, shadersCollection, mouseKeyboardCameraController.GetCamera());

        device.GetD3D11DeviceContext()->ClearState();

        device.GetD3D11DeviceContext()->CSSetShader(horBlurComputeShader.GetShader(), nullptr, 0);

        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, 0);

        device.GetD3D11DeviceContext()->Dispatch(UINT(ceil(float(backBufferSurface->GetWidth())/32.0f)), UINT(ceil(float(backBufferSurface->GetHeight()) / 32.0f)), 1);


        device.GetD3D11DeviceContext()->CSSetShader(verBlurComputeShader.GetShader(), nullptr, 0);

        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, 0);

        device.GetD3D11DeviceContext()->Dispatch(UINT(ceil(float(backBufferSurface->GetWidth()) / 32.0f)), UINT(ceil(float(backBufferSurface->GetHeight()) / 32.0f)), 1);

        device.GetD3D11DeviceContext()->ClearState();

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

