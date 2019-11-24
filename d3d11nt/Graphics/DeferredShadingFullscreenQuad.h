#pragma once
#include "System/Camera.h"
#include "Graphics/FullscreenQuad.h"
#include "Graphics/RenderSet.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "GraphicsConstantsBuffer.h"
#include "Data/Shaders/Test/deferredshadingpsconstants.h"
#include <d3d11.h>

class DeferredShadingFullscreenQuad
{
public:
    //DeferredShadingFullscreenQuad() {}
    DeferredShadingFullscreenQuad(GraphicsDevice& device, RenderSet& gbuffer, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection) : m_FullscreenQuad(shadersCollection), m_GBuffer(gbuffer), m_PixelShader(shadersCollection.GetShader<GraphicsPixelShader>(L"Test/deferredshadingps.hlsl", {}))
    {
        m_ConstantsBuffer = GraphicsConstantsBuffer<DeferredShadingPSConsts>(device);

        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;//D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
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

        m_EnvMap = textureCollection["cubemap.dds"];
    }

    void Render(GraphicsDevice& device, ShadersCollection& shadersCollection, Camera& camera, const glm::vec4 lightPos = glm::vec4(), const glm::vec3& f0override = glm::vec3(), const glm::vec3& diffuseOverride = glm::vec3(), float roverride = 0.0f, const std::vector<GraphicsShaderMacro> & psmacros = {})
    {
        m_PixelShader = shadersCollection.GetShader<GraphicsPixelShader>(L"Test/deferredshadingps.hlsl", psmacros);

        const glm::vec2 projFactors = camera.GetProjectionFactors();

        DeferredShadingPSConsts consts;
        consts.gbufferSize = glm::vec4(m_GBuffer.GetColorTexture(0).GetWidth(), m_GBuffer.GetColorTexture(0).GetHeight(), 0.0f, 0.0f);
        consts.vLightPos = camera.GetViewMatrix() * lightPos;
        consts.projFactors = glm::vec4(projFactors.x, projFactors.y, 0.0f, 0.0f);
        consts.invView = (glm::inverse(camera.GetViewMatrix()));
        consts.f0roughness = glm::vec4(f0override.r, f0override.g, f0override.b, roverride);
        consts.diffuseOverride = glm::vec4(diffuseOverride, 1.0);
        glm::vec3 pos = camera.GetPosition();
        consts.wCamPos = -glm::vec4(pos.x, pos.y, pos.z, 1.);

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
        textureSRVs.push_back(m_EnvMap->GetSRV());


        device.GetD3D11DeviceContext()->PSSetShaderResources(0, 4, textureSRVs.data());

        m_FullscreenQuad.Render(device);
    }

private:
    std::shared_ptr<Texture2D> m_EnvMap;

    GraphicsConstantsBuffer<DeferredShadingPSConsts> m_ConstantsBuffer;
    GraphicsPixelShader m_PixelShader;
    RenderSet& m_GBuffer;
    FullscreenQuad m_FullscreenQuad;
    ID3D11SamplerState* m_SamplerState;
};