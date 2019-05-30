#pragma once
#include "System/Camera.h"
#include "Graphics/FullscreenQuad.h"
#include "Graphics/RenderSet.h"
#include "GraphicsConstantsBuffer.h"
#include "Data/Shaders/Test/deferredshadingpsconstants.h"

class DeferredShadingFullscreenQuad
{
public:
    //DeferredShadingFullscreenQuad() {}
    DeferredShadingFullscreenQuad(GraphicsDevice& device, RenderSet& gbuffer, ShadersCollection& shadersCollection) : m_FullscreenQuad(shadersCollection), m_GBuffer(gbuffer), m_PixelShader(shadersCollection.GetShader<GraphicsPixelShader>(L"Test/deferredshadingps.hlsl", {}))
    {
        m_ConstantsBuffer = GraphicsConstantsBuffer<DeferredShadingPSConsts>(device);

        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        device.GetD3D11Device()->CreateSamplerState(&samplerDesc, &m_SamplerState);
    }

    void Render(GraphicsDevice& device, Camera& camera)
    {
        const glm::vec4 lightPos = glm::vec4(0.0, 0.0, -2.0, 1.0);

        DeferredShadingPSConsts consts;
        consts.vLightPos = lightPos * camera.GetViewMatrix();

        m_ConstantsBuffer.Update(device, consts);
        m_ConstantsBuffer.Bind(device, GraphicsShaderMask_Pixel, 0);

        m_PixelShader.Bind(device);
        device.GetD3D11DeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);

        std::vector<ID3D11ShaderResourceView*> textureSRVs;
        textureSRVs.reserve(3);
        for (size_t i = 0; i < 3; i++)
        {
            textureSRVs.push_back(m_GBuffer.GetColorTexture(i).GetSRV());
        }

        device.GetD3D11DeviceContext()->PSSetShaderResources(0, 3, textureSRVs.data());

        m_FullscreenQuad.Render(device);
    }

private:
    GraphicsConstantsBuffer<DeferredShadingPSConsts> m_ConstantsBuffer;
    GraphicsPixelShader m_PixelShader;
    RenderSet& m_GBuffer;
    FullscreenQuad m_FullscreenQuad;
    ID3D11SamplerState* m_SamplerState;
};