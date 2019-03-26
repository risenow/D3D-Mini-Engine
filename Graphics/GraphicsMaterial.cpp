#include "stdafx.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"
#include "Graphics/GraphicsMaterial.h"



std::string GraphicsMaterial::GetName() const
{
	return m_Name;
}

bool GraphicsMaterial::IsBatched() { return m_MaterialBatchStructuredBuffer.IsValid(); }

MaterialBatchStructuredBuffer GraphicsMaterial::m_MaterialBatchStructuredBuffer;