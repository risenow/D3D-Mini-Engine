#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/PlainColorMaterial.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"
#include <d3d11.h>

GraphicsConstantsBuffer<PSConsts> GraphicsPlainColorMaterial::m_ConstantsBuffer;
bool GraphicsPlainColorMaterial::m_ConstantsBufferInitialized = false;

GraphicsPlainColorMaterial::GraphicsPlainColorMaterial() {}
GraphicsPlainColorMaterial::GraphicsPlainColorMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element)
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
	//m_Data.coef = glm::vec3(redFactor, 0.0f, 0.0f);
	m_Name = name;
    m_Shader = shadersCollection.GetShader<GraphicsPixelShader>(L"Test/tessps.hlsl", { GraphicsShaderMacro("BATCH", "1") });//shadersCollection.GetShader<GraphicsPixelShader>(m_ShaderVariationIDs[0]);
	if (!m_ConstantsBufferInitialized)
	{
		m_ConstantsBuffer = GraphicsConstantsBuffer<PSConsts>(device);
		m_ConstantsBufferInitialized = true;
	}
}
void GraphicsPlainColorMaterial::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
{
	element->SetName(m_Type.c_str());
	element->SetAttribute("name", m_Name.c_str());

	tinyxml2::XMLElement* colorElement = document.NewElement("color");
	colorElement->SetAttribute("r", m_Data.coef.r);
	colorElement->SetAttribute("g", m_Data.coef.g);
	colorElement->SetAttribute("b", m_Data.coef.b);
	element->InsertEndChild(colorElement);
}
void GraphicsPlainColorMaterial::Deserialize(tinyxml2::XMLElement* element)
{
	const char* nameAttr = element->Attribute("name");
	assert(nameAttr);
	m_Name = nameAttr;
	tinyxml2::XMLElement* colorElement = element->FirstChildElement("color");
	assert(colorElement);
	m_Data.coef.r = colorElement->FloatAttribute("r");
	m_Data.coef.g = colorElement->FloatAttribute("g");
	m_Data.coef.b = colorElement->FloatAttribute("b");
}
void* GraphicsPlainColorMaterial::GetDataPtr()
{
	return &m_Data;
}
size_t GraphicsPlainColorMaterial::GetDataSize()
{
	return sizeof(PSConsts);
}
bool GraphicsPlainColorMaterial::HasValidConstantsBuffer() const
{
	return m_ConstantsBuffer.GetDX11Object() != nullptr;
}
GraphicsBuffer* GraphicsPlainColorMaterial::GetConstantsBuffer() const
{
	return (GraphicsBuffer*)&m_ConstantsBuffer;
}
void GraphicsPlainColorMaterial::Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex/* = 0*/)
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

	if (m_ConstantsBufferInitialized)
	{
		m_ConstantsBuffer.Update(device, m_Data);
		BindMultipleGraphicsConstantBuffers(device, 0, { &m_ConstantsBuffer }, GraphicsShaderMask_Pixel);
	}
}

GraphicsMaterial* GraphicsPlainColorMaterial::Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement)
{
	if (std::string(sceneGraphElement->Name()) != std::string("plain_color_material"))
		return nullptr;

	return (std::string(sceneGraphElement->Name()) == std::string("plain_color_material")) ? 
		popNew(GraphicsPlainColorMaterial)(device, textureCollection, shadersCollection, sceneGraphElement->Attribute("name"), sceneGraphElement) : nullptr;
}