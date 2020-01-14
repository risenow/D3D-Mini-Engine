#include "stdafx.h"
#include "Graphics/GraphicsMaterial.h"
#include "Graphics/BasicPixelShaderVariations.h"

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
void GraphicsMaterial::FillMaterialBits(uint32_t& dstMacros, const uint32_t passMacros)
{
    dstMacros = passMacros;
    if (IsBatched())
        dstMacros = dstMacros | BasicPixelShaderVariations::BATCH;
}