#include "stdafx.h"
#include "LightingGraphicsTopology.h"

void FillLightingGraphicsTopologyConstants(VSConsts& consts, const Camera& camera)
{
    consts.model = glm::mat4x4();
    consts.view = (camera.GetViewMatrix());
    consts.projection = (camera.GetProjectionMatrix());
    consts.normalMatrix = ((camera.GetViewMatrix()));
    consts.viewProjection = (camera.GetViewProjectionMatrix());
    consts.tessellationAmount = 5.0f;

    glm::vec3 pos = camera.GetPosition();
    consts.wpos = -glm::vec4(pos.x, pos.y, pos.z, 1.0);
}