#pragma once
#include "Extern/glm/gtx/common.hpp"
#include "GraphicsMaterial.h"
#include "GraphicsTopology.h"
#include "RenderSet.h"
#include "BasicGraphicsTopology.h"
//#include "Data/Shaders/Test/basicvsconstants.h"

#define IMMEDIATE_RENDER_MAX_VERTEXES 2048

struct BasicPSConstsDummy { float __p1; float __p2; float __p3; float __p4; };

//use for debug purposes only(at least in current state)
class ImmediateRenderer
{
public:
    struct VertexType
    {
        VertexType() {}
        VertexType(const glm::vec4& pos, const glm::vec4& color) : m_Position(pos), m_Color(color) {}
        glm::vec4 m_Position;
        glm::vec4 m_Color;
    };

    ImmediateRenderer();
    ImmediateRenderer(GraphicsDevice& device, ShadersCollection& shadersCollection);
    void OnFrameBegin(GraphicsDevice& device);
    void OnFrameEnd(GraphicsDevice& device, ShadersCollection& shadersCollection, Camera& camera, RenderSet& swapchainRenderSet); //render is here
    void Line(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& color); 
    void ReleaseGPUData();
    //void Render();
private:
    VertexData m_VertexData;
    size_t m_UsedVertexCount;
    BasicGraphicsMaterial<BasicPSConstsDummy> m_Material;
    TypedBasicVertexGraphicsTopology<BasicVSConsts> m_Topology;
};