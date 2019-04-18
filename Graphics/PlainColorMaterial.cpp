#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/PlainColorMaterial.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"

GraphicsConstantsBuffer<PSConsts> GraphicsPlainColorMaterial::m_ConstantsBuffer;
bool GraphicsPlainColorMaterial::m_ConstantsBufferInitialized = false;

GraphicsPlainColorMaterial::GraphicsPlainColorMaterial() {}
GraphicsPlainColorMaterial::GraphicsPlainColorMaterial(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element)//float redFactor)// :
	//m_ShaderID(GetShaderID(L"Test/ps.ps", { GraphicsShaderMacro("BATCH", "1") }))
{
	m_ShaderVariationIDs = { GetShaderID(L"Test/ps.hlsl", { GraphicsShaderMacro("BATCH", "1") }) };
    
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
void GraphicsPlainColorMaterial::Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, size_t variationIndex/* = 0*/)
{
	m_Shader = shadersCollection.GetShader<GraphicsPixelShader>(L"Test/tessps.hlsl", { GraphicsShaderMacro("BATCH", "1") });//shadersCollection.GetShader<GraphicsPixelShader>(m_ShaderVariationIDs[variationIndex]);
	m_Shader.Bind(device);

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

GraphicsMaterial* GraphicsPlainColorMaterial::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement)
{
	if (std::string(sceneGraphElement->Name()) != std::string("plain_color_material"))
		return nullptr;

	return (std::string(sceneGraphElement->Name()) == std::string("plain_color_material")) ? 
		popNew(GraphicsPlainColorMaterial)(device, shadersCollection, sceneGraphElement->Attribute("name"), sceneGraphElement) : nullptr;
}