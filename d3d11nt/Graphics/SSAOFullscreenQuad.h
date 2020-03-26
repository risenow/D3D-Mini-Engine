#pragma once
#include "System/Camera.h"
#include "Graphics/FullscreenQuad.h"
#include "Graphics/RenderSet.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "GraphicsConstantsBuffer.h"
#include "Data/Shaders/Test/ssaoconstants.h"
#include <d3d11.h>

class SSAOFullscreenQuad
{
public:
    glm::vec3 GenRandomVector() //req srand to be called previously
    {
        constexpr int HalfRandMax = RAND_MAX / 2;
        glm::vec3 v = glm::vec3(((float)rand() - (float)HalfRandMax) / (float)HalfRandMax, ((float)rand() - (float)HalfRandMax) / (float)HalfRandMax, (float)rand() / (float)RAND_MAX);
        //v = glm::normalize(v) * ((float)rand() / (float)RAND_MAX);
        return glm::normalize(v);
    }
    SSAOFullscreenQuad() : m_GBuffer(nullptr) {}
    SSAOFullscreenQuad(GraphicsDevice& device, RenderSet* gbuffer, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection) : m_FullscreenQuad(shadersCollection), m_GBuffer(gbuffer), m_PixelShader(shadersCollection.GetShader<GraphicsPixelShader>(L"Test/deferredshadingps.hlsl", {}))
    {
        m_ConstantsBuffer = GraphicsConstantsBuffer<SSAOPSConsts>(device);

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

        srand(time(nullptr));
        for (size_t i = 0; i < SSAO_SAMPLE_COUNT; i++)
        {

            //glm::vec3 v = glm::vec3(((float)rand() - (float)HalfRandMax) / (float)HalfRandMax, ((float)rand() - (float)HalfRandMax) / (float)HalfRandMax, (float)rand()/(float)RAND_MAX);
            //v = glm::normalize(v) * ((float)rand() / (float)RAND_MAX);
            glm::vec3 v;
            //do
            //{
                v = GenRandomVector();
            //} while (v.z < 0.05);
            consts.kernelSamples[i] = glm::vec4(v, 1.0);//glm::vec4(0.0, 0.0, 1.0, 1.0f);

            if (i < RAND_VECTOR_COUNT)
            {
                v = glm::normalize(glm::vec3(v.x, v.y, 0.0));
                consts.rv[i] = glm::vec4(v, 1.0f);
            }
        }
    }

    void Render(GraphicsDevice& device, ShadersCollection& shadersCollection, Camera& camera, uint32_t passBits = 0)
    {
        m_PixelShader = shadersCollection.GetShader<GraphicsPixelShader>(L"Test/ssaops.hlsl", passBits);

        const glm::vec2 projFactors = camera.GetProjectionFactors();

        popAssert(m_GBuffer);

        consts.gbufferSize = glm::vec4(m_GBuffer->GetColorTexture(0).GetWidth(), m_GBuffer->GetColorTexture(0).GetHeight(), 0.0f, 0.0f);
        consts.projFactors = glm::vec4(projFactors.x, projFactors.y, 0.0f, 0.0f);
        consts.projection = camera.GetProjectionMatrix();
        //consts.invProjection = glm::inverse(consts.projection);

        glm::vec4 pos = glm::vec4(2.0, 2.0, 2.0, 1.0);
        float z = pos.z;
        glm::vec4 posProj = camera.GetProjectionMatrix() * pos;
        posProj /= posProj.w;

        glm::vec4 reconstructed = glm::vec4(-posProj.x*projFactors.x*z, -posProj.y*projFactors.y*z, z, 1.0);

        m_ConstantsBuffer.Update(device, consts);
        m_ConstantsBuffer.Bind(device, GraphicsShaderMask_Pixel, 0);

        m_PixelShader.Bind(device);
        device.GetD3D11DeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);

        std::vector<ID3D11ShaderResourceView*> textureSRVs = { m_GBuffer->GetColorTexture(1).GetSRV(), m_GBuffer->GetColorTexture(0).GetSRV() };

        device.GetD3D11DeviceContext()->PSSetShaderResources(0, 2, textureSRVs.data());

        m_FullscreenQuad.Render(device);
    }

private:
    SSAOPSConsts consts;
    GraphicsConstantsBuffer<SSAOPSConsts> m_ConstantsBuffer;
    GraphicsPixelShader m_PixelShader;
    RenderSet* m_GBuffer;
    FullscreenQuad m_FullscreenQuad;
    ID3D11SamplerState* m_SamplerState;
};