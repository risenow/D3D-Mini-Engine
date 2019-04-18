#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/MaterialBatchStructuredBuffer.h"
#include "Graphics/dxlogicsafety.h"

MaterialBatchStructuredBuffer::MaterialBatchStructuredBuffer() : m_Data(nullptr), m_ElementSize(0), m_ElementsNum(0) {}
MaterialBatchStructuredBuffer::MaterialBatchStructuredBuffer(GraphicsDevice& device, const std::vector<GraphicsMaterial*>& materials) : m_Data(nullptr), m_ElementSize(0), m_ElementsNum(0)
{
	Update(device, materials);
}

void MaterialBatchStructuredBuffer::UpdateResources(GraphicsDevice& device)
{
    ID3D11DeviceContext* deviceContext = device.GetD3D11DeviceContext();
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    D3D_HR_OP(deviceContext->Map((ID3D11Resource*)m_StructuredBuffer.GetDX11Object(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
    memcpy(mappedResource.pData, m_Data, m_ElementSize * m_ElementsNum);
    deviceContext->Unmap((ID3D11Resource*)m_StructuredBuffer.GetDX11Object(), 0);
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
	static bool bufferNeedsToBeRecreated = false;

	if (m_Data && m_ElementSize != newElementSize && m_ElementsNum != newElementsNum)
	{
		popDelete(m_Data);

		//to replace this bizarre call to smth acceptable
		m_Data = malloc(newElementSize * newElementsNum);//MemoryManager::GetBigChunkAllocator()->Allocate(newElementSize*newElementSize, 64);

		bufferNeedsToBeRecreated = true;
	}

    if (!m_Data)
    {
        m_Data = malloc(newElementSize * newElementsNum);// MemoryManager::GetBigChunkAllocator()->Allocate(newElementSize * newElementSize, 64);
        bufferNeedsToBeRecreated = true;
    }

	m_ElementSize = newElementSize;
	m_ElementsNum = newElementsNum;

	byte_t* data = (byte_t*)m_Data;
    byte_t* dataDest = (byte_t*)data;
	for (size_t i = 0; i < m_ElementsNum; i++)
	{
        void* dataSrc = materials[i]->GetDataPtr();

        __strictmemcpy((void*)dataDest, dataSrc, newElementSize);
        dataDest += m_ElementSize;
	}

	if (bufferNeedsToBeRecreated)
    {
		m_StructuredBuffer = GraphicsBuffer(device, m_ElementsNum * m_ElementSize, GraphicsBuffer::BindFlag_Shader, GraphicsBuffer::UsageFlag_Dynamic, GraphicsBuffer::MiscFlag_Structured, m_Data, m_ElementsNum, m_ElementSize);

        UpdateResources(device);

        bufferNeedsToBeRecreated = false;
	}
	else
	{
		UpdateResources(device);
	}
}
void MaterialBatchStructuredBuffer::Bind(GraphicsDevice& device)
{
	//m_StructuredBuffer.Bind(device, GraphicsShaderMask_Vertex);
    //ID3D11ShaderResourceView* srv = m_StructuredBuffer.GetSRV();
    //ID3D11ShaderResourceView** srvs = (ID3D11ShaderResourceView * *)& srv;
    //device.GetD3D11DeviceContext()->VSSetShaderResources(0, 1, m_StructuredBuffer.GetSRV());
    //device.GetD3D11DeviceContext()->DSSetShaderResources(0, 1, srvs);
    //device.GetD3D11DeviceContext()->PSSetShaderResources(0, 1, m_StructuredBuffer.GetSRV());
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