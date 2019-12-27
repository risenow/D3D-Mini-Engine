#include "stdafx.h"
#include "ImmediateRenderer.h"
#include "BasicGraphicsTopology.h"

ImmediateRenderer::ImmediateRenderer(GraphicsDevice& device, ShadersCollection& shadersCollection) : m_UsedVertexCount(0)
{
    //m_Vertexes.reserve(IMMEDIATE_RENDER_MAX_VERTEXES);
    const std::string COLOR_PROPERTY_NAME = "COLOR";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec4>(COLOR_PROPERTY_NAME) });
    m_VertexData = VertexData(vertexFormat, IMMEDIATE_RENDER_MAX_VERTEXES);

    m_Topology = TypedBasicVertexGraphicsTopology<BasicVSConsts>(device, shadersCollection, ShaderStrIdentifier(L"Test/basicvs.hlsl", ShaderVariation()), m_VertexData, Topology_Lines, GraphicsBuffer::UsageFlag_Default, false);
    m_Material = BasicGraphicsMaterial< BasicPSConstsDummy>(device, shadersCollection, ShaderStrIdentifier(L"Test/basicps.hlsl", ShaderVariation()), "");
}

void ImmediateRenderer::OnFrameBegin(GraphicsDevice& device)
{
    m_Topology.UpdateVertexBuffers(device, m_VertexData);
    m_UsedVertexCount = 0;
}
void ImmediateRenderer::OnFrameEnd(GraphicsDevice& device, ShadersCollection& shadersCollection, Camera& camera, RenderSet& swapchainRenderSet)
{
    RenderSet renderSet = swapchainRenderSet;
    renderSet.SetSurfaces(renderSet.GetColorSurfaces(), nullptr);
    renderSet.Set(device);

    BasicVSConsts consts;
    FillBasicGraphicsTopologyConstants(consts, camera);

    m_Material.Bind(device, shadersCollection, camera, nullptr, {}, 0);
    m_Topology.Bind(device, shadersCollection, nullptr, nullptr, (void*)&consts);
    m_Topology.DrawCall(device, m_UsedVertexCount);
}

void ImmediateRenderer::Line(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& color)
{
    assert(m_UsedVertexCount + 2 < IMMEDIATE_RENDER_MAX_VERTEXES);
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_UsedVertexCount) = VertexType(v1, color);
    m_UsedVertexCount++;
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_UsedVertexCount) = VertexType(v2, color);
    m_UsedVertexCount++;
}

void ImmediateRenderer::ReleaseGPUData()
{
    m_Topology.ReleaseGPUData();
}