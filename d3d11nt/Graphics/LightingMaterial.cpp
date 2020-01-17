#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/LightingMaterial.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"
#include "Graphics/BasicPixelShaderVariations.h"
#include "Extern/tiny_obj_loader.h"
#include <d3d11.h>
#include <assimp/pbrmaterial.h>

PSConsts LightingConstsFromAiMaterial(aiMaterial* mat)
{
    PSConsts r;
    aiColor3D color(0.f, 0.f, 0.f);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    float rough;
    mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, rough);
    r.colorRoughness = glm::vec4(color.r, color.g, color.b, rough);
    return r;
}

std::string PreProcessPath(const std::string& _path, const std::string basePath)
{
    std::string path;
    path.reserve(_path.size() + basePath.size() + 1);
    path = basePath;
    if (path[path.size() - 1] != '/')
        path += '/';
    path += std::string(_path.c_str());

    return path;
}

std::vector<Texture2D*> ExtractLightingMaterialTexturesFromAiMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, aiMaterial* mat, const std::string& basePath)
{
    aiString diffusePath;
    mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffusePath);
    aiString normalPath;
    mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DISPLACEMENT, 0), normalPath);

    std::vector<Texture2D*> materials;
    Texture2D* mt = textureCollection.RequestTexture(device, PreProcessPath(std::string(diffusePath.C_Str()), basePath));

    if (!mt)
    {
        mt = textureCollection.GetBlackTexture();//textureCollection["Data/Textures/black.png"].get();
    }
    materials.push_back(mt);

    mt = textureCollection.RequestTexture(device, PreProcessPath(std::string(normalPath.C_Str()), basePath), true);

    if (!mt)
    {
        mt = textureCollection.GetBlackTexture();//textureCollection["Data/Textures/black.png"].get();
    }
    materials.push_back(mt);

    return materials;
}

GraphicsLightingMaterial::GraphicsLightingMaterial() {}
GraphicsLightingMaterial::GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const PSConsts& data, const std::vector<Texture2D*>& materialTextures)
    : TypedGraphicsMaterialBase<PSConsts>(device, shadersCollection, ShaderStrIdentifier(L"Test/ps.hlsl", 0), name)
{
    m_ShaderVariationIDs = { GetShaderID(L"Test/ps.hlsl", BasicPixelShaderVariations::BATCH) };

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    device.GetD3D11Device()->CreateSamplerState(&samplerDesc, &m_SamplerState);

    m_Cubemap = textureCollection["cubemap.dds"];

    popAssert(materialTextures.size() == 2);

    m_DiffuseMap = materialTextures[0];
    m_NormalMap = materialTextures[1];
    m_Data = data;
}
GraphicsLightingMaterial::GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element)
          : TypedGraphicsMaterialBase<PSConsts>(device, shadersCollection, ShaderStrIdentifier(L"Test/ps.hlsl", 0), name)
{
	m_ShaderVariationIDs = { GetShaderID(L"Test/ps.hlsl", BasicPixelShaderVariations::BATCH) };
    
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    device.GetD3D11Device()->CreateSamplerState(&samplerDesc, &m_SamplerState);

    m_DiffuseMap = textureCollection.GetBlackTexture();//textureCollection["Data/Textures/black.png"].get();
    m_NormalMap = m_DiffuseMap;
    m_Cubemap = textureCollection["cubemap.dds"];

    Deserialize(element);
}
void GraphicsLightingMaterial::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
{
	element->SetName(m_Type.c_str());
	element->SetAttribute("name", m_Name.c_str());

	tinyxml2::XMLElement* colorElement = document.NewElement("color");
	colorElement->SetAttribute("r", m_Data.colorRoughness.x);
	colorElement->SetAttribute("g", m_Data.colorRoughness.y);
	colorElement->SetAttribute("b", m_Data.colorRoughness.z);
	element->InsertEndChild(colorElement);
}
void GraphicsLightingMaterial::Deserialize(tinyxml2::XMLElement* element)
{
	const char* nameAttr = element->Attribute("name");
	assert(nameAttr);
	m_Name = nameAttr;
	tinyxml2::XMLElement* colorElement = element->FirstChildElement("color");
	assert(colorElement);
	m_Data.colorRoughness.x = colorElement->FloatAttribute("r");
	m_Data.colorRoughness.y = colorElement->FloatAttribute("g");
	m_Data.colorRoughness.z = colorElement->FloatAttribute("b");

    tinyxml2::XMLElement* pbrElement = element->FirstChildElement("pbr");
    m_Data.colorRoughness.w = pbrElement->FloatAttribute("roughness");
}

void GraphicsLightingMaterial::Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, uint32_t passBits, size_t variationIndex/* = 0*/)
{    
    m_Data.vLightPos = camera.GetViewMatrix() * m_Lights[0].m_LightPos;

    uint32_t shaderBits = passBits;
    _Bind(device, shadersCollection, shaderBits);

    std::vector< ID3D11ShaderResourceView*> textureSRVs = { m_DiffuseMap->GetSRV(), m_NormalMap->GetSRV() };
    device.GetD3D11DeviceContext()->PSSetShaderResources(0, 2, textureSRVs.data());
    device.GetD3D11DeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);
}
GraphicsMaterial* GraphicsLightingMaterial::Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement)
{
	if (std::string(sceneGraphElement->Name()) != std::string("lighting_material"))
		return nullptr;

	return (std::string(sceneGraphElement->Name()) == std::string("lighting_material")) ? 
		popNew(GraphicsLightingMaterial)(device, textureCollection, shadersCollection, sceneGraphElement->Attribute("name"), sceneGraphElement) : nullptr;
}