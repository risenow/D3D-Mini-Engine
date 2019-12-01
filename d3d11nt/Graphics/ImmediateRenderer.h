#pragma once
#include "Extern/glm/gtx/common.hpp"

class ImmediateRenderer
{
public:
    ImmediateRenderer();
    void OnFrameBegin();
    void OnFrameEnd();
    void Line(const glm::vec4& v1, const glm::vec4& v2); 
    void Render(); // renders lines using instanced single line buffer
private:
};