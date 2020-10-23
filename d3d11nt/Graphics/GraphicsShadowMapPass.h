#pragma once
#include "Graphics/RenderPass.h"
#include "Graphics/GraphicsDirectionalShadowMap.h"
#include "System/BasicVariablesContext.h"

class GraphicsShadowMapPass : public RenderPass
{
public:
    GraphicsShadowMapPass(GraphicsDevice& device, ID3D11DepthStencilState* depthState) : RenderPass({}, {}, "shadows"), m_ShadowMap(device, depthState)
    {
    }
    void Render(GraphicsDevice& device, SceneGraph& sceneGraph, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, Camera& camera, GraphicsOptions& options, void* data) override
    {
        BasicVariablesContext& basicVars = *(BasicVariablesContext*)data;
        m_ShadowMap.Render(device, shadersCollection, sceneGraph, basicVars.lightPos, glm::vec4(0.0, 0.0, 0.0, 2.0) - basicVars.lightPos);
    }

    GraphicsDirectionalShadowMap m_ShadowMap;
private:
    
};