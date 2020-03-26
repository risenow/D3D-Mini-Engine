// d3d11nt.cpp : Defines the entry point for the console application.
//

#pragma warning(disable:4996)
#include "stdafx.h"

#include <initguid.h>

#include "System/MemoryManager.h"
#include "System/Window.h"
#include "System/CommandLineArgs.h"
#include "System/Camera.h"
#include "System/MouseKeyboardCameraController.h"
#include "System/strutils.h"
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
#include "Graphics/GraphicsMaterialsManager.h"
#include "Graphics/DeferredShadingFullscreenQuad.h"
#include "Graphics/SSAOFullscreenQuad.h"
#include "Graphics/ImmediateRenderer.h"
#include "Graphics/BasicPixelShaderVariations.h"
#include "Graphics/BlurShaderVariations.h"
#include "Graphics/DeferredShadingShaderVariations.h"
#include "Graphics/SSAOShaderVariations.h"
#include "Graphics/RenderPass.h"
#include "Graphics/GBufferRenderPass.h"
#include "Graphics/SSAORenderPass.h"
#include "Graphics/LightingRenderPass.h"
#include "System/HardwareInfo.h"
#include "System/BasicVariablesContext.h"
#include "System/memutils.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtx/common.hpp"
#include "Extern/imgui/imgui.h"
#include "Extern/imgui/examples/imgui_impl_dx11.h"
#include "Extern/imgui/examples/imgui_impl_win32.h"
#include "Extern/tiny_obj_loader.h"
#include <d3d11.h>
#include <dxgidebug.h>
#include "Graphics/RenderSet.h"
#include "System/IniFile.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include "imguiConsole.h"

const std::string OPTIONS_INI_FILE_NAME = "options.ini";

void WriteFPSToWindowTitle(Window& window, const FrameMeasurer& frameMeasurer)
{
    std::string newWindowTitle = window.GetTitle();
    newWindowTitle += " FPS(per second counter): " + std::to_string(frameMeasurer.GetFPSBasedOnPerSecondFrameCounter());
    newWindowTitle += " FPS(based on average frametime): " + std::to_string(frameMeasurer.GetFPSBasedOnAverageFrameTime());
    SetWindowText(window.GetWindowHandle(), strtowstr_fast(newWindowTitle).c_str());
}

float DeserializeCameraFOV(const IniFile& ini)
{
	const std::string section = "Graphics";
	const float defaultFOV = 90;
	return ini.GetValue(section, "FOV", defaultFOV);
}

Camera CreateInitialCamera(const IniFile& ini, float aspect)
{
	const glm::vec3 position = glm::vec3(100.0f, .0f, .0f);
	const glm::vec3 rotation  = glm::vec3(0.0f, 0.0f, 0.0f);
	Camera camera = Camera(position, rotation);
    float deg = DeserializeCameraFOV(ini);
    camera.SetProjection(45.0f, aspect, 0.1f, 100000.0f);
	//camera.SetProjection(glm::perspective(glm::radians(DeserializeCameraFOV(ini)), 1.0f, 0.01f, 1000.0f));

	return camera;
}

class BasicVariablesCommandProducer : public Console::CommandProducer
{
public:
    static const std::string Signature;

    BasicVariablesCommandProducer() : Console::CommandProducer(Signature) {}

    class BasicVariablesCommand : public Console::Command
    {
    public:
        BasicVariablesCommand(const std::string& args) : Console::Command(BasicVariablesCommandProducer::Signature) 
        {
            ExtractWords(args, m_ArgWords);
        }
        void Execute(void* context)
        {
            BasicVariablesContext* ctx = (BasicVariablesContext*)context;

            if (m_ArgWords.size() < 2)
                return;

            if (m_ArgWords[0] == "light_int")
            {
                ctx->m_LightIntensity = atof(m_ArgWords[1].c_str());
            }
        }
    private:
        std::vector<std::string> m_ArgWords;
    };
    
    virtual Console::Command* Produce(const std::string& args)
    {
        return new BasicVariablesCommand(args);
        

        //if (words.size() < 2)
        //    return;
        //if (words[0] == "")
    }
};
const std::string BasicVariablesCommandProducer::Signature = "basicvar";
int main(int argc, char* argv[])
{
    CoInitialize(nullptr);
    HardwareInfo::GetInstance().Initialize(); // mb we should "unsignleton" this
    RuntimeLog::GetInstance().SetMode(RuntimeLogMode_WriteTime | RuntimeLogMode_WriteLine);

    AutoSaveIniFile iniFile(OPTIONS_INI_FILE_NAME, IniFile::LoadingFlags_FileMayNotExist);
    CommandLineArgs commandLineArgs(iniFile);
    FrameMeasurer frameMeasurer;
    GraphicsOptions options(iniFile);
    Window window(iniFile);
    MouseKeyboardCameraController mouseKeyboardCameraController(CreateInitialCamera(iniFile, (float)window.GetWidth()/(float)window.GetHeight()), iniFile);
    DisplayAdaptersList displayAdaptersList;
    D3D11DeviceCreationFlags deviceCreationFlags(true, false);//(commandLineArgs);

    BasicVariablesCommandProducer basicvarProducer;
    Console cons({ &basicvarProducer });

    int a;
    //std::cin >> a; // pause for injection

    GraphicsDevice device(deviceCreationFlags, FEATURE_LEVEL_ONLY_D3D11, nullptr);
    GraphicsSwapChain swapchain(device, window, MULTISAMPLE_TYPE_NONE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window.GetWindowHandle());
    ImGui_ImplDX11_Init(device.GetD3D11Device(), device.GetD3D11DeviceContext());

    ShadersCollection shadersCollection(device);
    shadersCollection.AddShader<GraphicsVertexShader>(L"Test/fsqvs.hlsl", { {} });
    shadersCollection.AddShader<GraphicsPixelShader>(L"Test/deferredshadingps.hlsl", DeferredShadingShaderVariations::GetPermutations());

    shadersCollection.AddShader<GraphicsPixelShader>(L"Test/ssaops.hlsl", SSAOShaderVariations::GetPermutations());

    shadersCollection.AddShader<GraphicsVertexShader>(L"Test/basicvs.hlsl", { {} });
    shadersCollection.AddShader<GraphicsPixelShader>(L"Test/basicps.hlsl", { {} });

    shadersCollection.AddShader<GraphicsVertexShader>(L"Test/vs.hlsl", GetAllPermutations({ GraphicsShaderMacro("BATCH", "1"), GraphicsShaderMacro("GBUFFER", "1"), GraphicsShaderMacro("TBN", "1"),  GraphicsShaderMacro("TEXCOORD", "1") }));
    shadersCollection.AddShader<GraphicsPixelShader>(L"Test/ps.hlsl", BasicPixelShaderVariations::GetPermutations());
    shadersCollection.AddShader<GraphicsComputeShader>(L"Test/cs.hlsl", BlurShaderVariations::GetPermutations());
                                                                                    
    shadersCollection.AddShader<GraphicsVertexShader> (L"Test/tessvs.hlsl", GetAllPermutations({ GraphicsShaderMacro("BATCH", "1") }));
    shadersCollection.AddShader<GraphicsHullShader>   (L"Test/tesshs.hlsl", GetAllPermutations({ GraphicsShaderMacro("BATCH", "1") }));
    shadersCollection.AddShader<GraphicsDomainShader> (L"Test/tessds.hlsl", GetAllPermutations({ GraphicsShaderMacro("BATCH", "1") }));
    shadersCollection.AddShader<GraphicsPixelShader>  (L"Test/tessps.hlsl", GetAllPermutations({ GraphicsShaderMacro("BATCH", "1") }));
    
    shadersCollection.ExecuteShadersCompilation(device);

    GraphicsTextureCollection textureCollection(device);
    //textureCollection.Add(device, "displ.png");
    textureCollection.Add(device, "cubemap.dds");
    //textureCollection.Add(device, "Data/Textures/black.png");

	SceneGraph sceneGraph(device, textureCollection, shadersCollection, "Data/scene.xml");

    ImmediateRenderer immediateRenderer(device, shadersCollection);

	FrameRateLock frameRateLock(100);

	window.AddCommand(SetCursorVisibilityCommand(false));

    ColorSurface backBufferSurface = swapchain.GetBackBufferSurface();

    Texture2D depthStencilTexture = Texture2DHelper::CreateCommonTexture(device,
                                                                         backBufferSurface.GetWidth(),
                                                                         backBufferSurface.GetHeight(),
                                                                         1,
                                                                         DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                                         options.GetMultisampleType(),
                                                                         D3D11_BIND_DEPTH_STENCIL);
    
    Texture2D intermediateBuffer = Texture2DHelper::CreateCommonTexture(device,
        backBufferSurface.GetWidth(),
        backBufferSurface.GetHeight(),
        1,
        backBufferSurface.GetTexture()->GetFormat(),
        options.GetMultisampleType(),
        D3D11_BIND_RENDER_TARGET);
    ColorSurface intermediateSurface = ColorSurface(device, &intermediateBuffer);
    //DEBUG_ONLY(depthStencilTexture.SetDebugName("Depth Stencil"));

    DepthSurface depthStencilSurface(device, &depthStencilTexture);
    RenderSet intermediateRenderSet({ &intermediateSurface }, &depthStencilSurface);
    if (options.GetMultisampleType() == MULTISAMPLE_TYPE_NONE)
        intermediateRenderSet.SetSurfaces({ &backBufferSurface }, &depthStencilSurface);

    RenderSet swapchainRenderSet({ &backBufferSurface }, &depthStencilSurface);

    RenderSet GBuffer = RenderSet(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight(), options.GetMultisampleType(), { DXGI_FORMAT_R32G32B32A32_FLOAT , DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT });//previously for normals DXGI_FORMAT_R32G32B32A32_FLOAT
    RenderSet SSAORenderSet = RenderSet(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight(), MULTISAMPLE_TYPE_NONE, { DXGI_FORMAT_R16G16B16A16_FLOAT }, true);

    DeferredShadingFullscreenQuad deferredShadingFullscreenQuad(device, &GBuffer, shadersCollection, textureCollection);
    SSAOFullscreenQuad ssaoFullscreenQuad(device, &GBuffer, shadersCollection, textureCollection);

    GraphicsViewport viewport(intermediateRenderSet);
    GraphicsViewport ssaoViewport(SSAORenderSet);

    swapchain.SetFullcreenState(false);

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    D3D11_RASTERIZER_DESC rastState;
    rastState.AntialiasedLineEnable = true;
    rastState.CullMode = D3D11_CULL_NONE;
    rastState.FillMode = D3D11_FILL_SOLID;
    rastState.FrontCounterClockwise = TRUE;
    rastState.DepthBias = 0;
    rastState.SlopeScaledDepthBias = 0.0f;
    rastState.DepthBiasClamp = 0.0f;
    rastState.DepthClipEnable = FALSE;
    rastState.ScissorEnable = FALSE;
    rastState.MultisampleEnable = options.GetMultisampleType() != MULTISAMPLE_TYPE_NONE;//FALSE;

    ID3D11RasterizerState* d3dRastState;

    D3D_HR_OP(device.GetD3D11Device()->CreateRasterizerState(&rastState, &d3dRastState));
    device.GetD3D11DeviceContext()->RSSetState(d3dRastState);

    
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;



    ID3D11DepthStencilState* FSQDepthStencilState;
    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilState(&depthStencilDesc, &FSQDepthStencilState));
    device.GetD3D11DeviceContext()->OMSetDepthStencilState(FSQDepthStencilState, 0);


    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ID3D11DepthStencilState* depthStencilState;
    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));
    device.GetD3D11DeviceContext()->OMSetDepthStencilState(depthStencilState, 0);

    GraphicsComputeShader horBlurComputeShader = shadersCollection.GetShader<GraphicsComputeShader>(L"Test/cs.hlsl", BlurShaderVariations::HORIZONTAL);
    GraphicsComputeShader verBlurComputeShader = shadersCollection.GetShader<GraphicsComputeShader>(L"Test/cs.hlsl", BlurShaderVariations::HORIZONTAL);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    ID3D11UnorderedAccessView* uav;
    uav = swapchain.GetBackBufferSurface().GetTexture()->GetUAV();
    
    int c = ShowCursor(true);

    bool deferredShading = true;
    bool pause = false;
    bool cameraRotationActive = false;

    PassResource GBufferResource = PassResource(&GBuffer, nullptr);
    GBufferRenderPass gbufferRenderPass = GBufferRenderPass(&GBufferResource, d3dRastState, depthStencilState);
    gbufferRenderPass.InitializeOutputDependencies();

    PassResource SSAOBufferResource = PassResource(&SSAORenderSet, nullptr);
    SSAORenderPass ssaoRenderPass = SSAORenderPass(device, shadersCollection, textureCollection, &SSAOBufferResource, &GBufferResource, d3dRastState, FSQDepthStencilState);
    ssaoRenderPass.InitializeOutputDependencies();

    PassResource finalResource = PassResource(&intermediateRenderSet, nullptr);
    LightingRenderPass lightingRenderPass = LightingRenderPass(device, shadersCollection, textureCollection, &finalResource, &SSAOBufferResource, &GBufferResource, d3dRastState, FSQDepthStencilState);
    lightingRenderPass.InitializeOutputDependencies();

    std::vector<RenderPass*> passList = BuildPassList(&finalResource);
    

    BasicVariablesContext basicVars;
    basicVars.m_LightIntensity = 1.0f;
    basicVars.f0override255 = glm::vec3(0.45f, 0.55f, 0.60f);
    basicVars.f0ampl = 0.1f;
    basicVars.f0overridetrunc;
    basicVars.useOptimizedSchlick = true;
    basicVars.overrideDiffuse = false;
    basicVars.lightPos = glm::vec4(100.0, 100.0, 100.0, 1.0);
    basicVars.roverride = 0.5;
    float fov = 45.0f;
    int msType = std::log2((int)options.GetMultisampleType()) + 1;
    bool ssaoToSwapchain = false;
    bool useAngleParametrization = false;
    float eyeX = 0.0f;
    float eyeY = 0.0f;

    while (!window.IsClosed())
    {
#ifndef MT_WINDOW
        if (window.ExplicitUpdate())
            continue;
#endif
        sceneGraph.GetMaterialManager()->Update({ {basicVars.lightPos, 1.0} });

        shadersCollection.ExecuteShadersCompilation(device);

		frameRateLock.OnFrameBegin();
        frameMeasurer.BeginMeasurement();

        if (GetAsyncKeyState(VK_RBUTTON))
        {
            cameraRotationActive = true;
        }
        else
            cameraRotationActive = false;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        immediateRenderer.OnFrameBegin(device);

        std::vector<Console::Command*> basicvarCommands;
        cons.FetchCommands(BasicVariablesCommandProducer::Signature, basicvarCommands);

        for (Console::Command* command : basicvarCommands)
        {
            ((BasicVariablesCommandProducer::BasicVariablesCommand*)command)->Execute((void*)&basicVars);
        }

        viewport = GraphicsViewport(window);

        if (!swapchain.IsValid(window, options.GetMultisampleType()) || intermediateRenderSet.GetColorSurface()->GetTexture()->GetSamplesCount() != (int)options.GetMultisampleType())
        {
            device.GetD3D11DeviceContext()->ClearState();
            swapchain.Validate(device, window, options.GetMultisampleType());

            backBufferSurface = swapchain.GetBackBufferSurface();
            if (options.GetMultisampleType() == MULTISAMPLE_TYPE_NONE)
            {
                depthStencilSurface.Resize(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight(), options.GetMultisampleType());

                intermediateRenderSet.SetSurfaces({ &backBufferSurface }, &depthStencilSurface);
            }
            else
            {
                intermediateRenderSet.SetSurfaces({ &intermediateSurface }, & depthStencilSurface);
                intermediateRenderSet.Resize(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight(), options.GetMultisampleType());
            }

            GBuffer.Resize(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight(), options.GetMultisampleType());

            SSAORenderSet.Resize(device, backBufferSurface.GetWidth(), backBufferSurface.GetHeight(), MULTISAMPLE_TYPE_NONE);
        }

        //gbuffer pass

        if (deferredShading)
        {
            if (ssaoToSwapchain)
            {
                ssaoRenderPass.SetOutputRenderSet(&intermediateRenderSet);
                finalResource.m_Dependency = &ssaoRenderPass;
                passList = BuildPassList(&finalResource);
            }
            else
            {
                ssaoRenderPass.SetOutputRenderSet(&SSAORenderSet);
                finalResource.m_Dependency = &lightingRenderPass;
                passList = BuildPassList(&finalResource);
            }
            for (RenderPass* pass : passList)
                pass->Render(device, sceneGraph, shadersCollection, textureCollection, mouseKeyboardCameraController.GetCamera(), options, &basicVars);

            /*
            uav = swapchain.GetBackBufferSurface().GetTexture()->GetUAV();
            device.GetD3D11DeviceContext()->ClearState();

            device.GetD3D11DeviceContext()->ClearState();

            device.GetD3D11DeviceContext()->CSSetShader(horBlurComputeShader.GetShader(), nullptr, 0);

            device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, 0);

            device.GetD3D11DeviceContext()->Dispatch(UINT(ceil(float(backBufferSurface.GetWidth()) / 32.0f)), UINT(ceil(float(backBufferSurface.GetHeight()) / 32.0f)), 1);


            device.GetD3D11DeviceContext()->CSSetShader(verBlurComputeShader.GetShader(), nullptr, 0);

            device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, 0);

            device.GetD3D11DeviceContext()->Dispatch(UINT(ceil(float(backBufferSurface.GetWidth()) / 32.0f)), UINT(ceil(float(backBufferSurface.GetHeight()) / 32.0f)), 1);

            device.GetD3D11DeviceContext()->ClearState();
            */
        }
    
        else
        {
            
            viewport.Set(device);
            intermediateRenderSet.Set(device);
            intermediateRenderSet.Clear(device, clearColor);
            device.GetD3D11DeviceContext()->RSSetState(d3dRastState);
            device.GetD3D11DeviceContext()->OMSetDepthStencilState(depthStencilState, 0);

            sceneGraph.GetOrdinaryGraphicsObjectManager()->Render(device, shadersCollection, { }, mouseKeyboardCameraController.GetCamera());

        }
        

        {
            static int counter = 0;

            ImGui::Begin("Lighting");
            ImGui::Checkbox("Camera Angle parametrization", &useAngleParametrization);
            ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f);
            ImGui::SliderFloat("Eye X", &eyeX, -1.0f, 1.0f);
            ImGui::SliderFloat("Eye Y", &eyeY, -1.0f, 1.0f);
            ImGui::Checkbox("Deferred", &deferredShading);
            ImGui::SliderFloat3("Light pos", (float*)&basicVars.lightPos, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Roughness", &basicVars.roverride, 0.0f, 1.0f);
            ImGui::Checkbox("Optimize Schlick", &basicVars.useOptimizedSchlick);
            ImGui::ColorEdit3("F0", (float*)& basicVars.f0override255);
            ImGui::SliderFloat("F0 ampl", &basicVars.f0ampl, 0.0f, 1.0f);
            ImGui::Checkbox("Override diffuse", &basicVars.overrideDiffuse);
            ImGui::Checkbox("SSAO to output", &ssaoToSwapchain);
            ImGui::ColorEdit3("Diffuse", (float*)& basicVars.diffuseOverride);
            std::string camPos = "X: " + std::to_string(mouseKeyboardCameraController.GetCamera().GetPosition().x) + "| Y:" + std::to_string(mouseKeyboardCameraController.GetCamera().GetPosition().y)+ "| Z:" + std::to_string(mouseKeyboardCameraController.GetCamera().GetPosition().z);
            ImGui::Text(camPos.c_str());
            if (ImGui::SliderInt("Multisample type", (int*)&msType, 1, 4))
                options.SetMultisampleType((MultisampleType)(unsigned int)(pow(2.0f, (float)msType - 1.0f)));
            basicVars.f0overridetrunc = basicVars.f0override255 * basicVars.f0ampl;
            mouseKeyboardCameraController.GetCamera().SetProjection(fov, window.GetWidth()/window.GetHeight(), 0.1f, 100000.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        cons.Process();

        ImGui::Render();
        intermediateRenderSet.Set(device);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (options.GetMultisampleType() > 1)
            device.GetD3D11DeviceContext()->ResolveSubresource(backBufferSurface.GetTexture()->GetD3D11Texture2D(), 0, intermediateBuffer.GetD3D11Texture2D(), 0, backBufferSurface.GetTexture()->GetFormat());

        //immediateRenderer.Line(glm::vec4(0.0, 0.0, 0.0, 1.0), glm::vec4(0.0, 0.0, -6.0, 1.0), glm::vec4(1.0, 0.0, 0.0, 1.0));
        //SSAO DEBUGING
        glm::vec3 n1 = glm::vec3(0.0, 0.0, 1.0);
        glm::vec3 r = glm::vec3(23.0, 12.0, 74.0);
        r = glm::normalize(r);
        glm::vec3 n2 = glm::normalize(r - glm::dot(n1, r) * n1);
        float res = glm::dot(n1, n2);

        immediateRenderer.Line(glm::vec4(0.0, 0.0, 0.0, 1.0), glm::vec4(n1, 1.0), glm::vec4(1.0, 0.0, 0.0, 1.0));
        immediateRenderer.Line(glm::vec4(0.0, 0.0, 0.0, 1.0), glm::vec4(r, 1.0), glm::vec4(1.0, 1.0, 0.0, 1.0));
        immediateRenderer.Line(glm::vec4(0.0, 0.0, 0.0, 1.0), glm::vec4(n2, 1.0), glm::vec4(0.0, 0.0, 1.0, 1.0));


        device.GetD3D11DeviceContext()->OMSetDepthStencilState(FSQDepthStencilState, 0);
        immediateRenderer.OnFrameEnd(device, shadersCollection, mouseKeyboardCameraController.GetCamera(), swapchainRenderSet);

        device.GetD3D11DeviceContext()->ClearState();

        if (swapchain.IsValid(window, options.GetMultisampleType())) //if swapchain is not valid discard the frame //POSSIBLE RACE CONDITION?
            swapchain.Present();

		frameRateLock.OnFrameEnd();
		frameMeasurer.EndMeasurement();

        WriteFPSToWindowTitle(window, frameMeasurer);

		if (window.IsFocused() && !pause)
		    mouseKeyboardCameraController.Update(window, useAngleParametrization, eyeX, eyeY);

        window.AddCommand(SetCursorVisibilityCommand(!mouseKeyboardCameraController.GetMouseCameraRotationActive()));
       
        if (pause)
            mouseKeyboardCameraController.GetCamera().UpdateViewProjectionMatrix();

        if (GetAsyncKeyState(0x50))
        {
            pause = !pause;
        }
        if (GetAsyncKeyState(192))
        {
            cons.Show();
        }
    }

    if (device.GetD3D11DeviceContext())
        device.GetD3D11DeviceContext()->ClearState();


    d3dRastState->Release();
    depthStencilState->Release();
    FSQDepthStencilState->Release();
    shadersCollection.ReleaseGPUData();
    backBufferSurface.ReleaseGPUData();
    depthStencilTexture.ReleaseGPUData();
    depthStencilSurface.ReleaseGPUData();

    swapchain.ReleaseGPUData();
    device.ReleaseGPUData();

    DEBUG_ONLY(device.ReportAllLiveObjects());

    window.SerializeToIni(iniFile);
    options.SerializeToIni(iniFile);
    return 0;
}

