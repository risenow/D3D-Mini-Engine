#pragma once
#include "System/Camera.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/GraphicsInputLayout.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/VertexData.h"
#include "Data/Shaders/Test/vsconstants.h"

class GraphicsTopology
{
public:
	GraphicsTopology();
	GraphicsTopology(GraphicsDevice& device, ShadersCollection& shadersCollection, VertexData& vertexData, bool isBatch = false);

	void Bind(GraphicsDevice& device, const Camera& camera);
	GraphicsBuffer* GetConstantsBuffer() const;

	void DrawCall(GraphicsDevice& device);
private:
	GraphicsVertexShader m_Shader;
	std::vector<VertexBuffer> m_VertexBuffers;
	GraphicsInputLayout m_InputLayout;

	unsigned long m_VertexCount;

	bool m_IsValid;

	static GraphicsConstantsBuffer<VSConsts> m_ConstantsBuffer;
	static bool m_ConstantsBufferInitialized;
};