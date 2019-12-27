#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/LightingMaterial.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"
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

GraphicsConstantsBuffer<PSConsts> GraphicsLightingMaterial::m_ConstantsBuffer;
bool GraphicsLightingMaterial::m_ConstantsBufferInitialized = false;

GraphicsLightingMaterial::GraphicsLightingMaterial() {}
GraphicsLightingMaterial::GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const PSConsts& data)
{
    m_ShaderVariationIDs = { GetShaderID(L"Test/ps.hlsl", { GraphicsShaderMacro("BATCH", "1") }) };

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
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

    m_Data = data;

    m_Name = name;
    if (!m_ConstantsBufferInitialized)
    {
        m_ConstantsBuffer = GraphicsConstantsBuffer<PSConsts>(device);
        m_ConstantsBufferInitialized = true;
    }
}
GraphicsLightingMaterial::GraphicsLightingMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element)
{
	m_ShaderVariationIDs = { GetShaderID(L"Test/ps.hlsl", { GraphicsShaderMacro("BATCH", "1") }) };
    
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
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

    Deserialize(element);

	m_Name = name;
	if (!m_ConstantsBufferInitialized)
	{
		m_ConstantsBuffer = GraphicsConstantsBuffer<PSConsts>(device);
		m_ConstantsBufferInitialized = true;
	}
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
void* GraphicsLightingMaterial::GetDataPtr()
{
	return &m_Data;
}
size_t GraphicsLightingMaterial::GetDataSize()
{
	return sizeof(PSConsts);
}
bool GraphicsLightingMaterial::HasValidConstantsBuffer() const
{
	return m_ConstantsBuffer.GetBuffer() != nullptr;
}
GraphicsBuffer* GraphicsLightingMaterial::GetConstantsBuffer() const
{
	return (GraphicsBuffer*)&m_ConstantsBuffer;
}
void GraphicsLightingMaterial::Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex/* = 0*/)
{
    std::vector<GraphicsShaderMacro> macros;
    macros.reserve(1 + passMacros.size());
    macros.push_back(GraphicsShaderMacro("BATCH", "1"));
    for (size_t i = 0; i < passMacros.size(); i++)
        macros.push_back(passMacros[i]);

	m_Shader = shadersCollection.GetShader<GraphicsPixelShader>(L"Test/ps.hlsl", macros);
	m_Shader.Bind(device);

    ID3D11ShaderResourceView* textureSRV = m_Cubemap->GetSRV();
    device.GetD3D11DeviceContext()->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView * *)& textureSRV);
    device.GetD3D11DeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);

    if (m_MaterialStructuredBuffer)
    {
        m_MaterialStructuredBuffer->Bind(device);
        return;
    }

    m_Data.vLightPos = camera.GetViewMatrix() * m_Lights[0].m_LightPos;

	if (m_ConstantsBufferInitialized)
	{
		m_ConstantsBuffer.Update(device, m_Data);
		BindMultipleGraphicsConstantBuffers(device, 0, { &m_ConstantsBuffer }, GraphicsShaderMask_Pixel);
	}
}
GraphicsMaterial* GraphicsLightingMaterial::Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement)
{
	if (std::string(sceneGraphElement->Name()) != std::string("lighting_material"))
		return nullptr;

	return (std::string(sceneGraphElement->Name()) == std::string("lighting_material")) ? 
		popNew(GraphicsLightingMaterial)(device, textureCollection, shadersCollection, sceneGraphElement->Attribute("name"), sceneGraphElement) : nullptr;
}