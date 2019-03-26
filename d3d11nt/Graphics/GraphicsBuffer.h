#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DX11Object.h"
#include "Graphics/VertexData.h"
#include "Graphics/GraphicsShader.h"

class GraphicsBuffer : public DX11MultipleObjectsHolder<IUnknown, 2>
{
private:
	static const index_t BUFFER_INDEX = 0;
	static const index_t SHADER_RESOURCE_INDEX = 1;

#define popBufferDX11Object m_DX11Objects[BUFFER_INDEX]
#define popShaderResourceDX11Object m_DX11Objects[SHADER_RESOURCE_INDEX]
public:
    enum BindFlags { BindFlag_Vertex, BindFlag_Index, BindFlag_Constant, BindFlag_Shader };
    enum UsageFlags { UsageFlag_Dynamic, UsageFlag_Staging, UsageFlag_Immutable, UsageFlag_Default };
    enum MiscFlags { MiscFlag_Default, MiscFlag_Structured };

	GraphicsBuffer() {}
    GraphicsBuffer(GraphicsDevice& device, size_t size, BindFlags bindFlag, 
                   UsageFlags usageFlag, MiscFlags miscFlag = MiscFlag_Default, 
                   void* data = nullptr, size_t structureByteStride = 0, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	void Bind(GraphicsDevice& device, GraphicsShaderMaskType stageMask);

	virtual ~GraphicsBuffer()
	{
		//ReleaseDX11Objects();
	}
protected:

    //void CreateDesc(const size_t size, const BindFlags bindFlag, 
    //                const UsageFlags usageFlag, const MiscFlags miscFlag, void* data, size_t structureByStride, 
    //                D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA& resourceData);
};

class VertexBuffer : public GraphicsBuffer
{
public:
	VertexBuffer();
    VertexBuffer(GraphicsDevice& device, VertexData& vertexData, index_t slotIndex = 0);

    void Bind(GraphicsDevice& device);
	size_t GetVertexSizeInBytes() const;
private:
    size_t m_VertexSizeInBytes;
};

void BindVertexBuffers(GraphicsDevice& device, const std::vector<VertexBuffer>& vertexBuffers);