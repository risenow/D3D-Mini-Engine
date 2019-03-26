#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"
#include "Graphics/dxlogicsafety.h"

MaterialBatchStructuredBuffer::MaterialBatchStructuredBuffer() : m_ElementSize(0) {}
MaterialBatchStructuredBuffer::MaterialBatchStructuredBuffer(GraphicsDevice& device, const std::vector<GraphicsMaterial*>& materials) : m_ElementSize(0)
{
	Update(device, materials);
}
void MaterialBatchStructuredBuffer::Update(GraphicsDevice& device, const std::vector<GraphicsMaterial*>& materials)
{
	if (!IsValidMaterialsSequenceForBatch(materials) || (!m_ElementSize && !m_ElementsNum && m_Data) || (m_ElementSize && m_ElementsNum && !m_Data))
	{
		assert(false);
		return;
	}

	size_t newElementSize = materials[0]->GetDataSize();
	size_t newElementsNum = materials.size();
	bool bufferNeedsToBeRecreated = false;

	if (m_Data && m_ElementSize != newElementSize && m_ElementsNum != newElementsNum)
	{
		popDelete(m_Data);

		//to replace this bizarre call to smth acceptable
		m_Data = MemoryManager::GetBigChunkAllocator()->Allocate(newElementSize*newElementSize, 64);

		bufferNeedsToBeRecreated = true;
	}

	m_ElementSize = newElementSize;
	m_ElementsNum = newElementsNum;

	byte_t* data = (byte_t*)m_Data;
	for (size_t i = 0; i < m_ElementsNum; i++)
	{
		memcpy(data, materials[i]->GetDataPtr(), m_ElementSize);
		data += m_ElementSize;
	}

	if (bufferNeedsToBeRecreated)
	{
		//m_StructuredBuffer.ReleaseDX11Objects();

		m_StructuredBuffer = GraphicsBuffer(device, m_ElementsNum * m_ElementSize, GraphicsBuffer::BindFlag_Shader, GraphicsBuffer::UsageFlag_Dynamic, GraphicsBuffer::MiscFlag_Structured, m_Data, m_ElementSize);
	}
	else
	{
		ID3D11DeviceContext* deviceContext = device.GetD3D11DeviceContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D_HR_OP(deviceContext->Map((ID3D11Resource*)m_StructuredBuffer.GetDX11Object(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		memcpy(mappedResource.pData, m_Data, m_ElementSize * m_ElementsNum);
		deviceContext->Unmap((ID3D11Resource*)m_StructuredBuffer.GetDX11Object(), 0);
	}
}
void MaterialBatchStructuredBuffer::Bind(GraphicsDevice& device)
{
	m_StructuredBuffer.Bind(device, GraphicsShaderMask_Pixel);
}

bool MaterialBatchStructuredBuffer::IsValidMaterialsSequenceForBatch(const std::vector<GraphicsMaterial*>& materials)
{
	if (!materials.size())
		return false;

	for (unsigned long i = 1; i < materials.size(); i++)
	{
		if (materials[i]->GetDataSize() != materials[0]->GetDataSize() && materials[i]->GetType() != materials[0]->GetType())
			return false;
	}
	return true;
}

extern std::map<unsigned int, std::shared_ptr<MaterialBatchStructuredBuffer>> MaterialBatchStructuredBuffers;