#pragma once
#include "Extern/glm/glm.hpp"
struct BasicVariablesContext
{
    float m_LightIntensity;
    glm::vec3 f0override255;
    float f0ampl;
    glm::vec3 f0overridetrunc;
    glm::vec3 diffuseOverride;
    glm::vec4 lightPos;
    float roverride;
    bool useOptimizedSchlick;
    bool overrideDiffuse;
};