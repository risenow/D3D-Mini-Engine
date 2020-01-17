#include "stdafx.h"
#include "Graphics/GraphicsTopology.h"
#include "System/Camera.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtx/matrix_major_storage.hpp"

GraphicsTopology::GraphicsTopology() : m_CapacityVertexCount(0), m_UsedVertexCount(0), m_IsBatch(false), m_IsValid(false) {}

//do not change shader input signature in real time or you'll be in trouble
GraphicsTopology::GraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, VertexData& data, GraphicsBuffer::UsageFlags usage, bool tcEnabled, bool tbnEnabled, bool isBatch) : m_TexCoordEnabled(tcEnabled), m_TBNEnabled(tbnEnabled), m_IsBatch(isBatch), m_IsValid(true), m_InputLayout(device, data.GetVertexFormat(), shadersCollection.GetShader<GraphicsVertexShader>(shaderStrIdentifier.path, shaderStrIdentifier.variation)),
m_CapacityVertexCount(data.GetNumVertexes()), m_UsedVertexCount(m_CapacityVertexCount)
{
    for (unsigned long i = 0; i < data.GetVertexFormat().GetNumSlotsUsed(); i++)
    {
        m_VertexBuffers.push_back(VertexBuffer(device, data, usage, i));
    }

    m_IndexesCount = data.GetIndexes().size();
    if (m_IndexesCount)
    {
        m_IndexBuffer = IndexBuffer(device, data.GetIndexes());
    }
}