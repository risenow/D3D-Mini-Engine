#pragma once
#include "BasicPixelShaderVariations.h"
#include "RenderPass.h"
#include "RenderSet.h"
#include "GraphicsViewport.h"

class GBufferRenderPass : public RenderPass
{
public:
    GBufferRenderPass(PassResource* gbufferRenderSet, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthState) : RenderPass({}, {gbufferRenderSet}, "gbuffer"), m_RastState(rastState), m_DepthState(depthState)
    {
        RenderSet* gbuffer = ((RenderSet*)m_OutputResources[0]->m_Resource);
        m_Viewport = GraphicsViewport(*gbuffer);
    }
    void Render(GraphicsDevice& device, SceneGraph& sceneGraph, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, Camera& camera, GraphicsOptions& options, void* data) override
    {
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

        RenderSet* gbuffer = ((RenderSet*)m_OutputResources[0]->m_Resource);
       
        gbuffer->Set(device);
        gbuffer->Clear(device, clearColor);
        device.GetD3D11DeviceContext()->RSSetState(m_RastState);
        device.GetD3D11DeviceContext()->OMSetDepthStencilState(m_DepthState, 0);

        m_Viewport = GraphicsViewport(*gbuffer);
        m_Viewport.Set(device);

        sceneGraph.GetOrdinaryGraphicsObjectManager()->Render(device, shadersCollection, BasicPixelShaderVariations::GBUFFER, camera);

        device.GetD3D11DeviceContext()->ClearState();
    }
private:
    ID3D11RasterizerState* m_RastState;
    ID3D11DepthStencilState* m_DepthState;
    GraphicsViewport m_Viewport;
};