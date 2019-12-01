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
    Topology_Lines,
    Topology_Tesselated,
    Topology_Basic
};

class GraphicsTopology
{
public:
    GraphicsTopology();
    GraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, VertexData& data,  bool isBatch);

    virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsBuffer* materialsStructuredbuffer, GraphicsBuffer* materialsConstantBuffer, void* constants) = 0;//const Camera& camera, TopologyType type) = 0;

    virtual void DrawCall(GraphicsDevice& device) = 0;
protected:
    std::vector<VertexBuffer> m_VertexBuffers;
    GraphicsInputLayout m_InputLayout;

    unsigned long m_VertexCount;

    bool m_IsValid;
    bool m_IsBatch;
};

template<class T>
class TypedBasicVertexTriangleGraphicsTopology : public GraphicsTopology
{
public:
    TypedBasicVertexTriangleGraphicsTopology(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, ShaderStrIdentifier vsShader, VertexData& vertexData, bool isBatch = false) : GraphicsTopology(device, shadersCollection, vsShader, vertexData, isBatch), m_ShaderStrIdentifier(vsShader)
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
    virtual void DrawCall(GraphicsDevice& device) override
    {
        device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device.GetD3D11DeviceContext()->Draw(m_VertexCount, 0);
    }
private:
    GraphicsVertexShader m_Shader;

    ShaderStrIdentifier m_ShaderStrIdentifier;

    static GraphicsConstantsBuffer<T> m_ConstantsBuffer;
    static bool m_ConstantsBufferInitialized;
};

template<class T>
GraphicsConstantsBuffer<T> TypedBasicVertexTriangleGraphicsTopology<T>::m_ConstantsBuffer;
template<class T>
bool TypedBasicVertexTriangleGraphicsTopology<T>::m_ConstantsBufferInitialized = false;