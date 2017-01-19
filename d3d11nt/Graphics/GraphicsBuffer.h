#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DX11Object.h"
#include "Graphics/VertexData.h"

class GraphicsBuffer : public D3D11ResourceHolder
{
public:
    enum BindFlags { BindFlag_Vertex, BindFlag_Index, BindFlag_Constant };
    enum UsageFlags { UsageFlag_Dynamic, UsageFlag_Staging, UsageFlag_Immutable, UsageFlag_Default };
    enum MiscFlags { MiscFlag_Default = 0 };

    GraphicsBuffer(GraphicsDevice& device, size_t size, BindFlags bindFlag, 
                   UsageFlags usageFlag, MiscFlags miscFlag = MiscFlag_Default, 
                   void* data = nullptr, size_t structureByteStride = 0);
protected:
    //void CreateDesc(const size_t size, const BindFlags bindFlag, 
    //                const UsageFlags usageFlag, const MiscFlags miscFlag, void* data, size_t structureByStride, 
    //                D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA& resourceData);
};

class VertexBuffer : public GraphicsBuffer
{
public:
    VertexBuffer(GraphicsDevice& device, const VertexData& vertexData);//void* data);

    void Bind(GraphicsDevice& device);
private:
    size_t m_VertexSizeInBytes;
};