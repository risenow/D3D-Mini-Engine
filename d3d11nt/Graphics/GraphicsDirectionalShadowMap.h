#pragma once
#include "System/Camera.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/RenderSet.h"
#include "Graphics/GraphicsTextureCollection.h"

class GraphicsDirectionalShadowMap
{
public:
    GraphicsDirectionalShadowMap(GraphicsDevice& device) : m_ShadowMapRenderSet(device, 512, 512, MULTISAMPLE_TYPE_NONE, {})
    {}

    Texture2D* GetShadowMapTexture()
    {
        return &m_ShadowMapRenderSet.GetDepthTexture();
    }

    void Render(GraphicsDevice& device, SceneGraph& sceneGraph);
private:
    RenderSet m_ShadowMapRenderSet;
    Camera m_Camera;
};