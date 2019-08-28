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
#include "Data/Shaders/Test/vsconstants.h"

enum TopologyType
{
    Topology_Tesselated,
    Topology_Basic
};

class GraphicsTopology
{
public:
	GraphicsTopology();
	GraphicsTopology(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, VertexData& vertexData, bool isBatch = false);

	void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsBuffer& buffer, const Camera& camera, TopologyType type);
	GraphicsBuffer* GetConstantsBuffer() const;

	void DrawCall(GraphicsDevice& device);
private:
	GraphicsVertexShader m_Shader;
    GraphicsVertexShader m_TessVSShader;
    GraphicsHullShader m_TessHSShader;
    GraphicsDomainShader m_TessDSShader;
    GraphicsPixelShader m_TessPSShader;
	std::vector<VertexBuffer> m_VertexBuffers;
	GraphicsInputLayout m_InputLayout;

    Texture2D* m_Texture;

	unsigned long m_VertexCount;

	bool m_IsValid;

	static GraphicsConstantsBuffer<VSConsts> m_ConstantsBuffer;
	static bool m_ConstantsBufferInitialized;
};