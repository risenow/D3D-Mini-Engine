#include "stdafx.h"
#include "BasicGraphicsTopology.h"

void FillBasicGraphicsTopologyConstants(BasicVSConsts& consts, const Camera& camera)
{
    consts.model = glm::mat4x4();
    consts.view = (camera.GetViewMatrix());
    consts.projection = (camera.GetProjectionMatrix());
}