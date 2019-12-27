#include "stdafx.h"
#include "Graphics/GraphicsMaterial.h"

std::string GraphicsMaterial::GetName() const
{
	return m_Name;
}

bool GraphicsMaterial::IsBatched() { return (m_MaterialStructuredBuffer) && m_MaterialStructuredBuffer->IsValid(); }

void GraphicsMaterial::SetMaterialsStructuredBuffer(std::shared_ptr<MaterialBatchStructuredBuffer> materialStructuredBuffer)
{
    m_MaterialStructuredBuffer = materialStructuredBuffer;
}

GraphicsBuffer& GraphicsMaterial::GetBuffer() { return m_MaterialStructuredBuffer->GetBuffer(); }

void GraphicsMaterial::Update(const std::vector<GraphicsLightObject>& lights)
{
    m_Lights = lights;
}
void GraphicsMaterial::FillMaterialMacros(std::vector<GraphicsShaderMacro>& dstMacros, const std::vector<GraphicsShaderMacro>& passMacros)
{
    dstMacros.reserve(1 + passMacros.size());
    if (IsBatched())
        dstMacros.push_back(GraphicsShaderMacro("BATCH", "1"));
    for (size_t i = 0; i < passMacros.size(); i++)
        dstMacros.push_back(passMacros[i]);
}