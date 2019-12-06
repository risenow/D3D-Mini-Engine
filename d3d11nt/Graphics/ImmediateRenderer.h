#pragma once
#include "Extern/glm/gtx/common.hpp"
#include "GraphicsTopology.h"
#include "Data/Shaders/Test/basicvsconstants.h"

class ImmediateRenderer
{
public:
    ImmediateRenderer();
    void OnFrameBegin();
    void OnFrameEnd();
    void Line(const glm::vec4& v1, const glm::vec4& v2); 
    void Render();
private:
    TypedBasicVertexGraphicsTopology<BasicVSConsts> m_Topology;
};