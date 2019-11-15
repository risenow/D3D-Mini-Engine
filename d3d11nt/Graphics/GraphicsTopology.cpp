#include "stdafx.h"
#include "Graphics/GraphicsTopology.h"
#include "Graphics/GeneralShaderMutationsDefines.h"
#include "System/Camera.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtx/matrix_major_storage.hpp"

GraphicsConstantsBuffer<VSConsts> GraphicsTopology::m_ConstantsBuffer;
bool GraphicsTopology::m_ConstantsBufferInitialized = false;

GraphicsTopology::GraphicsTopology() : m_VertexCount(0), m_IsBatch(false), m_IsValid(false) {}
GraphicsTopology::GraphicsTopology(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, VertexData& vertexData, bool isBatch) : m_IsBatch(isBatch),// m_VertexBuffer(device, vertexData),
    m_Shader(shadersCollection.GetShader<GraphicsVertexShader>(L"Test/vs.hlsl", isBatch ? ShaderVariation({ SHADER_MACRO_BATCH }) : ShaderVariation())),
    m_TessVSShader(shadersCollection.GetShader<GraphicsVertexShader>(L"Test/tessvs.hlsl", ShaderVariation({ SHADER_MACRO_BATCH }))),
    m_TessHSShader(shadersCollection.GetShader<GraphicsHullShader>(L"Test/tesshs.hlsl", ShaderVariation({ SHADER_MACRO_BATCH }))),
    m_TessDSShader(shadersCollection.GetShader<GraphicsDomainShader>(L"Test/tessds.hlsl", ShaderVariation({ SHADER_MACRO_BATCH }))),
    m_Texture(textureCollection["displ.png"].get()),

    m_InputLayout(device, vertexData.GetVertexFormat(), m_Shader),
    m_VertexCount(vertexData.GetNumVertexes())
{
	for (unsigned long i = 0; i < vertexData.GetVertexFormat().GetNumSlotsUsed(); i++)
	{
		m_VertexBuffers.push_back(VertexBuffer(device, vertexData, i));
	}
	if (!m_ConstantsBufferInitialized)
	{
		m_ConstantsBuffer = GraphicsConstantsBuffer<VSConsts>(device);
		m_ConstantsBufferInitialized = true;
	}
}

void GraphicsTopology::Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsBuffer* materialsStructuredbuffer, GraphicsBuffer* materialsConstantBuffer, const Camera& camera, TopologyType type)
{
    m_Shader = shadersCollection.GetShader<GraphicsVertexShader>(L"Test/vs.hlsl", m_IsBatch ? ShaderVariation({ SHADER_MACRO_BATCH }) : ShaderVariation({}));
    ID3D11ShaderResourceView* bufferSRV = materialsStructuredbuffer ? materialsStructuredbuffer->GetSRV() : nullptr;
    switch (type)
    {
    case Topology_Tesselated:
    {
        m_TessVSShader.Bind(device);
        m_TessHSShader.Bind(device);
        m_TessDSShader.Bind(device);
        m_InputLayout.Bind(device);

        ID3D11ShaderResourceView* textureSRV = m_Texture->GetSRV();
        device.GetD3D11DeviceContext()->DSSetShaderResources(0, 1, (ID3D11ShaderResourceView * *)& textureSRV);

        //index for resource binding to be calculated by state manager
        if (bufferSRV)
            device.GetD3D11DeviceContext()->DSSetShaderResources(1, 1, (ID3D11ShaderResourceView * *)& bufferSRV);
        break;
    }
    case Topology_Basic:
        m_Shader.Bind(device);
        m_InputLayout.Bind(device);
    }

    if (bufferSRV)
        device.GetD3D11DeviceContext()->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView * *)& bufferSRV);

    BindVertexBuffers(device, m_VertexBuffers);

	if (m_ConstantsBufferInitialized)
	{
		VSConsts consts;
        //consts.translatedview.Set(camera.translatedview);
        consts.model = glm::mat4x4();
        consts.view = (camera.GetViewMatrix());
        consts.projection = (camera.GetProjectionMatrix());
        consts.normalMatrix  = ((camera.GetViewMatrix()));
        consts.viewProjection = (camera.GetViewProjectionMatrix());
        consts.tessellationAmount = 5.0f;

        glm::vec3 pos = camera.GetPosition();
        consts.wpos = -glm::vec4(pos.x, pos.y, pos.z, 1.0);//glm::vec4( camera.GetPosition(), 1.0);
		m_ConstantsBuffer.Update(device, consts);

        std::vector<GraphicsBuffer*> cbuffers = { &m_ConstantsBuffer };
        if (materialsConstantBuffer)
            cbuffers.push_back(materialsConstantBuffer);

        BindMultipleGraphicsConstantBuffers(device, 0, cbuffers, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel | GraphicsShaderMask_Hull | GraphicsShaderMask_Domain);
	}
}
GraphicsBuffer* GraphicsTopology::GetConstantsBuffer() const
{
	return &m_ConstantsBuffer;
}

void GraphicsTopology::DrawCall(GraphicsDevice& device)
{
    device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	device.GetD3D11DeviceContext()->Draw(m_VertexCount, 0);
}