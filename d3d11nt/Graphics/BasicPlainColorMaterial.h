#pragma once
#include "GraphicsMaterial.h"
#include "ShadersCollection.h"

class GraphicsBasicPlainColorMaterial : public GraphicsMaterial
{
public:
    GraphicsBasicPlainColorMaterial() {}

    void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex = 0)
    {
        GraphicsVertexShader vs = shadersCollection.GetShader(L"Test/basicVs", {});
        vs.Bind(device);
    }
    static GraphicsMaterial* Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement);
};