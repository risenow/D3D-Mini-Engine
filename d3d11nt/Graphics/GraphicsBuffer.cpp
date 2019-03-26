#include "stdafx.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/dxlogicsafety.h"
#include "System/logicsafety.h"

unsigned int BindFlagsToD3D11BindFlags(GraphicsBuffer::BindFlags bindFlags)
{
    switch (bindFlags)
    {
    case GraphicsBuffer::BindFlag_Vertex:
        return D3D11_BIND_VERTEX_BUFFER;
        break;
    case GraphicsBuffer::BindFlag_Index:
        return D3D11_BIND_INDEX_BUFFER;
        break;
    case GraphicsBuffer::BindFlag_Constant:
        return D3D11_BIND_CONSTANT_BUFFER;
        break;
	case GraphicsBuffer::BindFlag_Shader:
		return D3D11_BIND_SHADER_RESOURCE;
		break;
    default:
        popAssert(false);
        return -1;
        break;
    }
}

void CalculateD3D11UsageAndCPUAccessFlags(const GraphicsBuffer::UsageFlags usageFlags, D3D11_USAGE& d3d11UsageFlags, unsigned int& d3d11CPUAccessFlags)
{
    switch (usageFlags)
    {
    case GraphicsBuffer::UsageFlag_Default:
        d3d11UsageFlags = D3D11_USAGE_DEFAULT;
        d3d11CPUAccessFlags = 0;
        break;
    case GraphicsBuffer::UsageFlag_Immutable:
        d3d11UsageFlags = D3D11_USAGE_IMMUTABLE;
        d3d11CPUAccessFlags = 0;
        break;
    case GraphicsBuffer::UsageFlag_Dynamic:
        d3d11UsageFlags = D3D11_USAGE_DYNAMIC;
        d3d11CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        break;
    case GraphicsBuffer::UsageFlag_Staging:
        d3d11UsageFlags = D3D11_USAGE_STAGING;
        d3d11CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        break;
    default:
        popAssert(false);
        break;
    }
}

unsigned int MiscFlagToD3D11MiscFlag(GraphicsBuffer::MiscFlags miscFlag)
{
    switch (miscFlag)
    {
    case GraphicsBuffer::MiscFlag_Default:
        return 0;
        break;
	case GraphicsBuffer::MiscFlag_Structured:
		return D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		break;
    default:
        popAssert(false);
        return -1;
        break;
    }
}

void CreateDesc(const size_t size, const GraphicsBuffer::BindFlags bindFlag,
                const GraphicsBuffer::UsageFlags usageFlag, const GraphicsBuffer::MiscFlags miscFlag, void* data, size_t structureByteStride,
                D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA& resourceData)
{
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = BindFlagsToD3D11BindFlags(bindFlag);
    bufferDesc.MiscFlags = MiscFlagToD3D11MiscFlag(miscFlag);
    bufferDesc.StructureByteStride = structureByteStride;

    CalculateD3D11UsageAndCPUAccessFlags(usageFlag, bufferDesc.Usage, bufferDesc.CPUAccessFlags);

    resourceData.pSysMem = data;
    resourceData.SysMemPitch = 0;
    resourceData.SysMemSlicePitch = 0;
}

void CreateSRVDesc(size_t size, size_t structureByteStride, DXGI_FORMAT format, D3D11_SHADER_RESOURCE_VIEW_DESC& desc)
{
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = format;

	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = structureByteStride ? structureByteStride : size;
}

GraphicsBuffer::GraphicsBuffer(GraphicsDevice& device, size_t size, BindFlags bindFlag,
                                UsageFlags usageFlag, MiscFlags miscFlag/* = MiscFlag_Default*/,
                                void* data/* = nullptr*/, size_t structureByteStride/* = 0*/, DXGI_FORMAT format/* = DXGI_FORMAT_UNKNOWN*/)
{
    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA subresourceData;

    CreateDesc(size, bindFlag, usageFlag, miscFlag, data, structureByteStride, bufferDesc, subresourceData);
    D3D_HR_OP(device.GetD3D11Device()->CreateBuffer(&bufferDesc, (data) ? &subresourceData : nullptr, (ID3D11Buffer**)&popBufferDX11Object.GetRef()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	CreateSRVDesc(size, structureByteStride, format, srvDesc);

	//D3D_HR_OP(device.GetD3D11Device()->CreateShaderResourceView((ID3D11Resource*)popBufferDX11Object.Get(), &srvDesc, (ID3D11ShaderResourceView**)&popShaderResourceDX11Object.GetRef()));
}
void GraphicsBuffer::Bind(GraphicsDevice& device, GraphicsShaderMaskType stageMask) 
{
	assert(stageMask != 0);

	void(__stdcall ID3D11DeviceContext::*setShaderResources[GraphicsShaderType_Count])(UINT, UINT, ID3D11ShaderResourceView*const*);
	setShaderResources[GraphicsShaderType_Vertex] = &ID3D11DeviceContext::VSSetShaderResources;
	setShaderResources[GraphicsShaderType_Pixel] = &ID3D11DeviceContext::PSSetShaderResources;

	for (unsigned long i = 0; i < GraphicsShaderType_Count; i++)
	{
		if (stageMask & (1u << i))
			(device.GetD3D11DeviceContext()->*setShaderResources[i])(0, 1, (ID3D11ShaderResourceView**)&popShaderResourceDX11Object.GetRef());
	}
}

VertexBuffer::VertexBuffer() {}
VertexBuffer::VertexBuffer(GraphicsDevice& device, VertexData& vertexData, index_t slotIndex) : GraphicsBuffer(device, 
                                                                                                  vertexData.GetSizeInBytesForSlot(slotIndex) * vertexData.GetNumVertexes(),
                                                                                                  BindFlag_Vertex, UsageFlag_Immutable,
                                                                                                  MiscFlag_Default, vertexData.GetDataPtrForSlot(slotIndex)),
                                                                                                  m_VertexSizeInBytes(vertexData.GetVertexFormat().GetVertexSizeInBytesForSlot(slotIndex))
{
    //popAssert(vertexData.GetVertexFormat().GetNumSlotsUsed() == 1); // vertex buffer per slot

}
void VertexBuffer::Bind(GraphicsDevice& device)
{
	BindVertexBuffers(device, { *this });
    //offset_t offset = 0;
    //device.GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&GetDX11ObjectReference(), &m_VertexSizeInBytes, (unsigned int*)&offset);
}

size_t VertexBuffer::GetVertexSizeInBytes() const
{
	return m_VertexSizeInBytes;
}

void BindVertexBuffers(GraphicsDevice& device, const std::vector<VertexBuffer>& vertexBuffers)
{
	UINT* offsets = (UINT*)alloca(sizeof(UINT)*vertexBuffers.size());
	size_t* sizesPerVertex = (size_t*)alloca(sizeof(size_t)*vertexBuffers.size());
	ID3D11Buffer** buffers = (ID3D11Buffer**)alloca(sizeof(ID3D11Buffer*)*vertexBuffers.size());
	for (unsigned long i = 0; i < vertexBuffers.size(); i++)
	{
		offsets[i] = 0;
		sizesPerVertex[i] = vertexBuffers[i].GetVertexSizeInBytes();
		buffers[i] = (ID3D11Buffer*)vertexBuffers[i].GetDX11Object();
	}

	device.GetD3D11DeviceContext()->IASetVertexBuffers(0, vertexBuffers.size(), buffers, sizesPerVertex, offsets);
}