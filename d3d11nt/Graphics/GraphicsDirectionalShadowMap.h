#pragma once
#include "System/Camera.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/GraphicsViewport.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/RenderSet.h"
//#include <d3d11.h>

class GraphicsDirectionalShadowMap
{
public:
    GraphicsDirectionalShadowMap(GraphicsDevice& device, ID3D11DepthStencilState* depthState) : m_ShadowMapRenderSet(device, 2048*2, 2048*2, MULTISAMPLE_TYPE_NONE, {}), m_DepthState(depthState)
    {
        m_Camera.SetProjectionAsOrtho(-500, 500, -500, 500, 10.0, 5500.0);
        //m_Camera.SetProjection(70.0, 1.0, 1.0, 10000.0);

        D3D11_BLEND_DESC blendStateDesc;
        blendStateDesc.AlphaToCoverageEnable = false;
        blendStateDesc.IndependentBlendEnable = false;
        blendStateDesc.RenderTarget[0].BlendEnable = false;
        blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
        blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_COLOR;
        blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0;
        blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
        blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_COLOR;

        D3D11_RASTERIZER_DESC rastStateDesc;
        rastStateDesc.AntialiasedLineEnable = true;
        rastStateDesc.CullMode = D3D11_CULL_NONE;
        rastStateDesc.FillMode = D3D11_FILL_SOLID;
        rastStateDesc.FrontCounterClockwise = TRUE;
        rastStateDesc.DepthBias = 0;
        rastStateDesc.SlopeScaledDepthBias = 0.0f;
        rastStateDesc.DepthBiasClamp = 0.0f;
        rastStateDesc.DepthClipEnable = FALSE;
        rastStateDesc.ScissorEnable = FALSE;
        rastStateDesc.MultisampleEnable = FALSE;

        device.GetD3D11Device()->CreateRasterizerState(&rastStateDesc, &m_RastState);
        device.GetD3D11Device()->CreateBlendState(&blendStateDesc, &m_BlendState);
    }

    Texture2D* GetShadowMapTexture()
    {
        return &m_ShadowMapRenderSet.GetDepthTexture();
    }

    void Render(GraphicsDevice& device, ShadersCollection& shadersCollection, SceneGraph& sceneGraph, const glm::vec3& lightPosition, const glm::vec3& lightDirection)
    {
        m_Camera.SetPosition(lightPosition);
        m_Camera.SetOrientation(lightDirection);

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

        m_ShadowMapRenderSet.Set(device);
        m_ShadowMapRenderSet.Clear(device, clearColor);
        device.GetD3D11DeviceContext()->RSSetState(m_RastState);
        device.GetD3D11DeviceContext()->OMSetBlendState(m_BlendState, clearColor, 0xffffffff);
        device.GetD3D11DeviceContext()->OMSetDepthStencilState(m_DepthState, 0);

        GraphicsViewport viewport = GraphicsViewport(m_ShadowMapRenderSet);
        viewport.Set(device);

        sceneGraph.GetOrdinaryGraphicsObjectManager()->Render(device, shadersCollection, 0, m_Camera, true);

        device.GetD3D11DeviceContext()->ClearState();
    }

    Camera& GetCamera()
    {
        return m_Camera;
    }
private:
    ID3D11RasterizerState* m_RastState;
    ID3D11DepthStencilState* m_DepthState;
    ID3D11BlendState* m_BlendState;
    RenderSet m_ShadowMapRenderSet;
    Camera m_Camera;
};