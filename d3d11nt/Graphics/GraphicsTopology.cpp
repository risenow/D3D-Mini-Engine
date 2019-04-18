#include "stdafx.h"
#include "Graphics/GraphicsTopology.h"
#include "Graphics/GeneralShaderMutationsDefines.h"

GraphicsConstantsBuffer<VSConsts> GraphicsTopology::m_ConstantsBuffer;
bool GraphicsTopology::m_ConstantsBufferInitialized = false;

GraphicsTopology::GraphicsTopology() : m_VertexCount(0) {}
GraphicsTopology::GraphicsTopology(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, VertexData& vertexData, bool isBatch) : // m_VertexBuffer(device, vertexData),
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

void GraphicsTopology::Bind(GraphicsDevice& device, GraphicsBuffer& buffer, const Camera& camera, TopologyType type)
{
    ID3D11ShaderResourceView* bufferSRV = buffer.GetSRV();
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

        device.GetD3D11DeviceContext()->DSSetShaderResources(1, 1, (ID3D11ShaderResourceView * *)& bufferSRV);
        break;
    }
    case Topology_Basic:
        m_Shader.Bind(device);
        m_InputLayout.Bind(device);
    }

    device.GetD3D11DeviceContext()->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView * *)& bufferSRV);

    BindVertexBuffers(device, m_VertexBuffers);

	//m_VertexBuffer.Bind(device);
	if (m_ConstantsBufferInitialized)
	{
		VSConsts consts;
		consts.viewProjection = camera.GetViewProjectionMatrix();
        consts.tessellationAmount = 5.0f;

		m_ConstantsBuffer.Update(device, consts);
        BindMultipleGraphicsConstantBuffers(device, 0, { &m_ConstantsBuffer }, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel | GraphicsShaderMask_Hull | GraphicsShaderMask_Domain);
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