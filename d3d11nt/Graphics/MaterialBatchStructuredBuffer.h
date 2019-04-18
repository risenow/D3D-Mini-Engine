#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/GraphicsMaterial.h"
#include <map>
#include <vector>
#include <memory>

class MaterialBatchStructuredBuffer
{
public:
	MaterialBatchStructuredBuffer();
	MaterialBatchStructuredBuffer(GraphicsDevice& device, const std::vector<GraphicsMaterial*>& materials);

	void Bind(GraphicsDevice& device);
	void Update(GraphicsDevice& device, const std::vector<GraphicsMaterial*>& materials);

    GraphicsBuffer& GetBuffer() { return m_StructuredBuffer; }

	bool IsValid() { return m_ElementSize > 0 && m_ElementsNum > 0; }
private:
    void UpdateResources(GraphicsDevice& device);
	bool IsValidMaterialsSequenceForBatch(const std::vector<GraphicsMaterial*>& materials);

	size_t m_ElementSize;
	size_t m_ElementsNum;
	void* m_Data;
	GraphicsBuffer m_StructuredBuffer;
};

//extern std::map<unsigned int, std::shared_ptr<MaterialBatchStructuredBuffer>> MaterialBatchStructuredBuffers;
//struct GraphicsMaterials
//{

//};