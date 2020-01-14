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
std::vector<Texture2D*> ExtractLightingMaterialTexturesFromAiMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, aiMaterial* mat, const std::string& basePath);

class GraphicsLightingMaterial : public TypedGraphicsMaterialBase<PSConsts>
{
public:
	GraphicsLightingMaterial();
    //GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, aiMaterial* mat);
    GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const PSConsts& data, const std::vector<Texture2D*>& materialTextures, bool tcEnabled = false, bool tbn = false);
    GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element, bool tcEnabled = false, bool tbn = false);//float redFactor);
	GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const std::vector<GraphicsMaterial*> batchedMaterials);

	void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, uint32_t passBits, size_t variationIndex = 0) override;

	virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) override;
	virtual void Deserialize(tinyxml2::XMLElement* element) override;

	static GraphicsMaterial* Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement);
private:
	ShaderID m_ShaderID;

    ID3D11SamplerState* m_SamplerState;

    Texture2D* m_DiffuseMap;
    Texture2D* m_NormalMap; // mb make blue(?) imposter for this map in case of it is apsent
    std::shared_ptr<Texture2D> m_Cubemap;

    bool m_TBNEnabled;
    bool m_TexCoordEnabled;
};