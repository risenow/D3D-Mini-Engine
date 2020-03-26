#include "stdafx.h"
#include "ImmediateRenderer.h"
#include "BasicGraphicsTopology.h"
#include "GraphicsViewport.h"

ImmediateRenderer::ImmediateRenderer(GraphicsDevice& device, ShadersCollection& shadersCollection) : m_LinesUsedVertexCount(0), m_PointsUsedVertexCount(0)
{
    //m_Vertexes.reserve(IMMEDIATE_RENDER_MAX_VERTEXES);
    const std::string COLOR_PROPERTY_NAME = "COLOR";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec4>(COLOR_PROPERTY_NAME) });
    m_VertexData = VertexData(vertexFormat, IMMEDIATE_RENDER_MAX_VERTEXES);

    m_Topology = TypedBasicVertexGraphicsTopology<BasicVSConsts>(device, shadersCollection, ShaderStrIdentifier(L"Test/basicvs.hlsl", 0), m_VertexData, Topology_Lines, GraphicsBuffer::UsageFlag_Default, false);
    m_Material = BasicGraphicsMaterial< BasicPSConstsDummy>(device, shadersCollection, ShaderStrIdentifier(L"Test/basicps.hlsl", 0), "");
    m_PointsStartLocation = IMMEDIATE_RENDER_MAX_VERTEXES / 2;
}

void ImmediateRenderer::OnFrameBegin(GraphicsDevice& device)
{
    m_Topology.UpdateVertexBuffers(device, m_VertexData);
    m_LinesUsedVertexCount = 0;
    m_PointsUsedVertexCount = 0;
}
void ImmediateRenderer::OnFrameEnd(GraphicsDevice& device, ShadersCollection& shadersCollection, Camera& camera, RenderSet& swapchainRenderSet)
{
    GraphicsViewport viewport = GraphicsViewport(swapchainRenderSet);

    RenderSet renderSet = swapchainRenderSet;
    renderSet.SetSurfaces(renderSet.GetColorSurfaces(), nullptr);
    renderSet.Set(device);

    viewport.Set(device);

    BasicVSConsts consts;
    FillBasicGraphicsTopologyConstants(consts, camera);

    m_Material.Bind(device, shadersCollection, camera, nullptr, {}, 0);
    m_Topology.Bind(device, shadersCollection, nullptr, nullptr, (void*)&consts);
    m_Topology.DrawCall(device, m_LinesUsedVertexCount);
    m_Topology.DrawCallEx(device, Topology_Points, m_PointsUsedVertexCount, m_PointsStartLocation);
}

void ImmediateRenderer::Line(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& color)
{
    assert(m_LinesUsedVertexCount + 2 < m_PointsStartLocation);
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_LinesUsedVertexCount) = VertexType(v1, color);
    m_LinesUsedVertexCount++;
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_LinesUsedVertexCount) = VertexType(v2, color);
    m_LinesUsedVertexCount++;
}
void ImmediateRenderer::Point(const glm::vec4& v, const glm::vec4& color)
{
    assert(m_PointsUsedVertexCount + 1 < IMMEDIATE_RENDER_MAX_VERTEXES);
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_PointsUsedVertexCount) = VertexType(v, color);
    m_PointsUsedVertexCount++;
}

void ImmediateRenderer::ReleaseGPUData()
{
    m_Topology.ReleaseGPUData();
}