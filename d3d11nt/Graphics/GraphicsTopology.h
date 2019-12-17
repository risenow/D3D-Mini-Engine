#pragma once
#include "System/Camera.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/GraphicsInputLayout.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "Graphics/VertexData.h"
#include "Graphics/GeneralShaderMutationsDefines.h"

enum TopologyType
{   
    Topology_Lines = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
    Topology_Triangles = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
};

//may be reset state after draw?
class GraphicsTopology
{
public:
    GraphicsTopology();
    GraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, VertexData& data, GraphicsBuffer::UsageFlags flags,  bool isBatch);

    virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsBuffer* materialsStructuredbuffer, GraphicsBuffer* materialsConstantBuffer, void* constants) = 0;//const Camera& camera, TopologyType type) = 0;
    //virtual void ReleaseGPUData() = 0;
    virtual void DrawCall(GraphicsDevice& device, int overrideVertexCount = -1) = 0;

    void ReleaseGPUData()
    {
        for (VertexBuffer& buffer : m_VertexBuffers)
            buffer.ReleaseGPUData();
        m_InputLayout.ReleaseGPUData();
    }
    void UpdateVertexBuffers(GraphicsDevice& device, VertexData& data)
    {
        assert(!m_IndexBuffer.GetBuffer());
        if (data.GetNumVertexes() == 0)
        {
            m_UsedVertexCount = 0;
            return;
        }
        popAssert(data.GetVertexFormat().GetNumSlotsUsed() == m_VertexBuffers.size());
        popAssert(data.GetNumVertexes() <= m_CapacityVertexCount);

        //use map unmap
        device.GetD3D11DeviceContext()->UpdateSubresource(m_VertexBuffers[0].GetBuffer(), 0, nullptr, data.GetDataPtrForSlot(0), data.GetSizeInBytesForSlot(0), 1);
    }

    std::vector<VertexBuffer>& VertexBuffers() { return m_VertexBuffers; }
protected:
    IndexBuffer m_IndexBuffer;
    std::vector<VertexBuffer> m_VertexBuffers;
    GraphicsInputLayout m_InputLayout;

    unsigned long m_CapacityVertexCount;
    unsigned long m_UsedVertexCount; 
    unsigned long m_IndexesCount;

    bool m_IsValid;
    bool m_IsBatch;
};

//add possibility to create dynamic vertex buffer
template<class T>
class TypedBasicVertexGraphicsTopology : public GraphicsTopology
{
public:
    TypedBasicVertexGraphicsTopology() 
    {}
    TypedBasicVertexGraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, ShaderStrIdentifier vsShader, VertexData& vertexData, TopologyType type, GraphicsBuffer::UsageFlags usage, bool isBatch = false) : GraphicsTopology(device, shadersCollection, vsShader, vertexData, usage, isBatch), m_ShaderStrIdentifier(vsShader), m_Type(type)
    {
        if (!m_ConstantsBufferInitialized)
        {
            m_ConstantsBuffer = GraphicsConstantsBuffer<T>(device);
            m_ConstantsBufferInitialized = true;
        }
    }
    virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsBuffer* materialsStructuredbuffer, GraphicsBuffer* materialsConstantBuffer, void* constants) override
    {
        m_Shader = shadersCollection.GetShader<GraphicsVertexShader>(m_ShaderStrIdentifier.path, m_IsBatch ? ShaderVariation({ SHADER_MACRO_BATCH }) : ShaderVariation({}));
        ID3D11ShaderResourceView* bufferSRV = materialsStructuredbuffer ? materialsStructuredbuffer->GetSRV() : nullptr;
        
        m_Shader.Bind(device);
        m_InputLayout.Bind(device);

        if (bufferSRV)
            device.GetD3D11DeviceContext()->VSSetShaderResources(0, 1, (ID3D11ShaderResourceView * *)& bufferSRV);

        BindVertexBuffers(device, m_VertexBuffers);
        m_IndexBuffer.Bind(device);

        if (m_ConstantsBufferInitialized)
        {
            T consts = *((T*)constants);

            m_ConstantsBuffer.Update(device, consts);

            std::vector<GraphicsBuffer*> cbuffers = { &m_ConstantsBuffer };
            if (materialsConstantBuffer)
                cbuffers.push_back(materialsConstantBuffer);

            BindMultipleGraphicsConstantBuffers(device, 0, cbuffers, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel | GraphicsShaderMask_Hull | GraphicsShaderMask_Domain);
        }
    }
    virtual void DrawCall(GraphicsDevice& device, int overrideVertexCount = -1) override
    {
        device.GetD3D11DeviceContext()->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)m_Type);
        if (m_IndexesCount)
        {
            device.GetD3D11DeviceContext()->DrawIndexed(m_IndexesCount, 0, 0);
            return;
        }
        device.GetD3D11DeviceContext()->Draw(overrideVertexCount >= 0 ? overrideVertexCount : m_UsedVertexCount, 0);
    }
private:
    TopologyType m_Type;

    GraphicsVertexShader m_Shader;

    ShaderStrIdentifier m_ShaderStrIdentifier;

    static GraphicsConstantsBuffer<T> m_ConstantsBuffer;
    static bool m_ConstantsBufferInitialized;
};

template<class T>
GraphicsConstantsBuffer<T> TypedBasicVertexGraphicsTopology<T>::m_ConstantsBuffer;
template<class T>
bool TypedBasicVertexGraphicsTopology<T>::m_ConstantsBufferInitialized = false;