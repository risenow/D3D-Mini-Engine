#include "stdafx.h"
#include "Graphics/GraphicsTopology.h"
#include "Graphics/GeneralShaderMutationsDefines.h"

GraphicsConstantsBuffer<VSConsts> GraphicsTopology::m_ConstantsBuffer;
bool GraphicsTopology::m_ConstantsBufferInitialized = false;

GraphicsTopology::GraphicsTopology() : m_VertexCount(0) {}
GraphicsTopology::GraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, VertexData& vertexData, bool isBatch) : // m_VertexBuffer(device, vertexData),
	m_Shader(shadersCollection.GetShader<GraphicsVertexShader>(L"Test/vs.vs", isBatch ? ShaderVariation({ SHADER_MACRO_BATCH }) : ShaderVariation())),
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

void GraphicsTopology::Bind(GraphicsDevice& device, const Camera& camera)
{
	m_Shader.Bind(device);
	m_InputLayout.Bind(device);
	BindVertexBuffers(device, m_VertexBuffers);
	//m_VertexBuffer.Bind(device);
	if (m_ConstantsBufferInitialized)
	{
		VSConsts consts;
		consts.viewProjection = camera.GetViewProjectionMatrix();

		m_ConstantsBuffer.Update(device, consts);
		BindMultipleGraphicsConstantBuffers(device, 0, { &m_ConstantsBuffer }, GraphicsShaderMask_Vertex);
	}
}
GraphicsBuffer* GraphicsTopology::GetConstantsBuffer() const
{
	return &m_ConstantsBuffer;
}

void GraphicsTopology::DrawCall(GraphicsDevice& device)
{
	device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	device.GetD3D11DeviceContext()->Draw(m_VertexCount, 0);
}