#pragma once
#include "BasicPixelShaderVariations.h"
#include "RenderPass.h"
#include "RenderSet.h"
#include "GraphicsViewport.h"
#include "DeferredShadingFullscreenQuad.h"
#include "DeferredShadingShaderVariations.h"
#include "System/BasicVariablesContext.h"

class LightingRenderPass : public RenderPass
{
public:
    LightingRenderPass(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, PassResource* finalRenderSet, PassResource* ssaoRenderSet, PassResource* gbufferRenderSet, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthState) : RenderPass({ gbufferRenderSet, ssaoRenderSet }, { finalRenderSet }, "lighting"), m_RastState(rastState), m_DepthState(depthState)
    {
        RenderSet* final = ((RenderSet*)m_OutputResources[0]->m_Resource);
        RenderSet* gbuffer = ((RenderSet*)m_InputResources[0]->m_Resource);
        RenderSet* SSAORenderSet = ((RenderSet*)m_InputResources[1]->m_Resource);

        popAssert(gbuffer && SSAORenderSet);

        m_Viewport = GraphicsViewport(*final);
        m_LightingFullscreenQuad = DeferredShadingFullscreenQuad(device, gbuffer, shadersCollection, textureCollection);
    }
    void Render(GraphicsDevice& device, SceneGraph& sceneGraph, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, Camera& camera, GraphicsOptions& options, void* data) override
    {
        popAssert(data);
        BasicVariablesContext* basicVariables = (BasicVariablesContext*)data;

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

        RenderSet* final = ((RenderSet*)m_OutputResources[0]->m_Resource);
        RenderSet* gbuffer = ((RenderSet*)m_InputResources[0]->m_Resource);
        RenderSet* SSAORenderSet = ((RenderSet*)m_InputResources[1]->m_Resource);

        
        final->Set(device);
        final->Clear(device, clearColor);
        device.GetD3D11DeviceContext()->RSSetState(m_RastState);
        device.GetD3D11DeviceContext()->OMSetDepthStencilState(m_DepthState, 0);
        m_Viewport = GraphicsViewport(*final);
        m_Viewport.Set(device);

        uint32_t passBits = 0;

        if (basicVariables->useOptimizedSchlick)
            passBits = passBits | DeferredShadingShaderVariations::OPTIMIZED_SCHLICK;
        if (basicVariables->overrideDiffuse)
            passBits = passBits | DeferredShadingShaderVariations::OVERRIDE_DIFFUSE;

        if (options.GetMultisampleType() > 1)
        {
            switch (options.GetMultisampleType())
            {
            case 2:
            {
                passBits = passBits | DeferredShadingShaderVariations::SAMPLES_COUNT2;
                break;
            }
            case 4:
            {
                passBits = passBits | DeferredShadingShaderVariations::SAMPLES_COUNT4;
                break;
            }
            case 8:
            {
                passBits = passBits | DeferredShadingShaderVariations::SAMPLES_COUNT8;
                break;
            }
            }
        }

        m_LightingFullscreenQuad.Render(device, shadersCollection, camera, &SSAORenderSet->GetColorTexture(0), basicVariables->lightPos, basicVariables->f0overridetrunc, basicVariables->diffuseOverride, basicVariables->roverride, passBits);

        device.GetD3D11DeviceContext()->ClearState();
    }
private:
    DeferredShadingFullscreenQuad m_LightingFullscreenQuad;
    ID3D11RasterizerState* m_RastState;
    ID3D11DepthStencilState* m_DepthState;
    GraphicsViewport m_Viewport;
};