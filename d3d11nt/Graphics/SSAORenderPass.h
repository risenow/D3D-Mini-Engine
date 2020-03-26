#pragma once
#include "BasicPixelShaderVariations.h"
#include "RenderPass.h"
#include "RenderSet.h"
#include "GraphicsViewport.h"
#include "SSAOShaderVariations.h"
#include "SSAOFullscreenQuad.h"
#include "BlurShaderVariations.h"

class SSAORenderPass : public RenderPass
{
public:
    SSAORenderPass(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, PassResource* ssaoRenderSet, PassResource* gbufferRenderSet, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthState) : RenderPass({gbufferRenderSet}, {ssaoRenderSet}, "ssao"), m_RastState(rastState), m_DepthState(depthState)
    {
        RenderSet* gbuffer = ((RenderSet*)m_InputResources[0]->m_Resource);
        RenderSet* SSAORenderSet = ((RenderSet*)m_OutputResources[0]->m_Resource);

        popAssert(gbuffer && SSAORenderSet);

        m_Viewport = GraphicsViewport(*SSAORenderSet);
        m_SSAOFullscreenQuad = SSAOFullscreenQuad(device, gbuffer, shadersCollection, textureCollection);
    }
    void SetOutputRenderSet(RenderSet* outputRenderSet)
    {
        m_OutputResources[0]->m_Resource = (void*)outputRenderSet;
    }
    void Render(GraphicsDevice& device, SceneGraph& sceneGraph, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, Camera& camera, GraphicsOptions& options, void* data) override
    {
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

        RenderSet* gbuffer = ((RenderSet*)m_InputResources[0]->m_Resource);
        RenderSet* ssaoRenderSet = ((RenderSet*)m_OutputResources[0]->m_Resource);

        ssaoRenderSet->Set(device);
        ssaoRenderSet->Clear(device, clearColor);
        device.GetD3D11DeviceContext()->RSSetState(m_RastState);
        device.GetD3D11DeviceContext()->OMSetDepthStencilState(m_DepthState, 0);

        m_Viewport = GraphicsViewport(*ssaoRenderSet);
        m_Viewport.Set(device);

        uint32_t passBits = 0;

        if (options.GetMultisampleType() > 1)
        {
            switch (options.GetMultisampleType())
            {
            case 2:
            {
                passBits = passBits | SSAOShaderVariations::SAMPLES_COUNT2;
                break;
            }
            case 4:
            {
                passBits = passBits | SSAOShaderVariations::SAMPLES_COUNT4;
                break;
            }
            case 8:
            {
                passBits = passBits | SSAOShaderVariations::SAMPLES_COUNT8;
                break;
            }
            }
        }

        m_SSAOFullscreenQuad.Render(device, shadersCollection, camera, passBits);

        GraphicsComputeShader horBlurComputeShader = shadersCollection.GetShader<GraphicsComputeShader>(L"Test/cs.hlsl", BlurShaderVariations::HORIZONTAL);
        GraphicsComputeShader verBlurComputeShader = shadersCollection.GetShader<GraphicsComputeShader>(L"Test/cs.hlsl", BlurShaderVariations::VERTICAL);

        ID3D11UnorderedAccessView* uav = ssaoRenderSet->GetColorSurface()->GetTexture()->GetUAV();

        device.GetD3D11DeviceContext()->ClearState();

        device.GetD3D11DeviceContext()->ClearState();

        device.GetD3D11DeviceContext()->CSSetShader(horBlurComputeShader.GetShader(), nullptr, 0);

        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, 0);

        device.GetD3D11DeviceContext()->Dispatch(UINT(ceil(float(ssaoRenderSet->GetWidth()) / 32.0f)), UINT(ceil(float(ssaoRenderSet->GetHeight()) / 32.0f)), 1);


        device.GetD3D11DeviceContext()->CSSetShader(verBlurComputeShader.GetShader(), nullptr, 0);

        device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, 0);

        device.GetD3D11DeviceContext()->Dispatch(UINT(ceil(float(ssaoRenderSet->GetWidth()) / 32.0f)), UINT(ceil(float(ssaoRenderSet->GetHeight()) / 32.0f)), 1);

        device.GetD3D11DeviceContext()->ClearState();
    }
private:
    SSAOFullscreenQuad m_SSAOFullscreenQuad;
    ID3D11RasterizerState* m_RastState;
    ID3D11DepthStencilState* m_DepthState;
    GraphicsViewport m_Viewport;
};