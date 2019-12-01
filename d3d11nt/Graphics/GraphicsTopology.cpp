#include "stdafx.h"
#include "Graphics/GraphicsTopology.h"
#include "System/Camera.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtx/matrix_major_storage.hpp"

GraphicsTopology::GraphicsTopology() : m_VertexCount(0), m_IsBatch(false), m_IsValid(false) {}

//do not change shader input signature in real time or you'll be in trouble
GraphicsTopology::GraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, VertexData& data, bool isBatch) : m_IsBatch(isBatch), m_IsValid(true), m_InputLayout(device, data.GetVertexFormat(), shadersCollection.GetShader<GraphicsVertexShader>(shaderStrIdentifier.path, shaderStrIdentifier.variation)),
m_VertexCount(data.GetNumVertexes())
{
    for (unsigned long i = 0; i < data.GetVertexFormat().GetNumSlotsUsed(); i++)
    {
        m_VertexBuffers.push_back(VertexBuffer(device, data, i));
    }
}