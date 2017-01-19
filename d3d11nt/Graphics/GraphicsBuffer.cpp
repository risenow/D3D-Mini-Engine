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

GraphicsBuffer::GraphicsBuffer(GraphicsDevice& device, size_t size, BindFlags bindFlag,
                                UsageFlags usageFlag, MiscFlags miscFlag/* = MiscFlag_Default*/,
                                void* data/* = nullptr*/, size_t structureByteStride/* = 0*/)
{
    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA subresourceData;
    CreateDesc(size, bindFlag, usageFlag, miscFlag, data, structureByteStride, bufferDesc, subresourceData);
    D3D_HR_OP(device.GetD3D11Device()->CreateBuffer(&bufferDesc, &subresourceData, (ID3D11Buffer**)&m_DX11Object.GetRef()));
}

VertexBuffer::VertexBuffer(GraphicsDevice& device, const VertexData& vertexData) : GraphicsBuffer(device, 
                                                                                                  vertexData.GetSizeInBytesForSlot(0) * vertexData.GetNumVertexes(),
                                                                                                  BindFlag_Vertex, UsageFlag_Immutable,
                                                                                                  MiscFlag_Default, vertexData.GetDataPtrForSlot(0)), 
                                                                                                  m_VertexSizeInBytes(vertexData.GetVertexFormat().GetVertexSizeInBytesForSlot(0))
{
    popAssert(vertexData.GetVertexFormat().GetNumSlotsUsed() == 1); // vertex buffer per slot

}
void VertexBuffer::Bind(GraphicsDevice& device)
{
    offset_t offset = 0;
    device.GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&m_DX11Object.GetRef(), &m_VertexSizeInBytes, (unsigned int*)&offset);
}