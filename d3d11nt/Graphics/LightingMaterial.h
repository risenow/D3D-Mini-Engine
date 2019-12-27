#pragma once
#include <string>
#include "Extern/tinyxml2.h"
#include "Extern/tiny_obj_loader.h"
#include "Graphics/GraphicsMaterial.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "System/Camera.h"
#include "Data/Shaders/Test/psconstants.h"
#include <assimp/material.h>

PSConsts LightingConstsFromAiMaterial(aiMaterial* mat);

class GraphicsLightingMaterial : public TypedGraphicsMaterialBase<PSConsts>
{
public:
	GraphicsLightingMaterial();
    GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const PSConsts& data);
    GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element);//float redFactor);
	GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const std::vector<GraphicsMaterial*> batchedMaterials);

	void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex = 0) override;

	virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) override;
	virtual void Deserialize(tinyxml2::XMLElement* element) override;

	static GraphicsMaterial* Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement);
private:
	ShaderID m_ShaderID;

    ID3D11SamplerState* m_SamplerState;

    std::shared_ptr<Texture2D> m_Cubemap;
};