#pragma once
#include <string>
#include "Graphics/GraphicsShader.h"
#include "Graphics/GraphicsBuffer.h"
#include "Graphics/GraphicsInputLayout.h"
#include "Graphics/GraphicsMaterial.h"
#include "Graphics/GraphicsTopology.h"
#include "MaterialBatchStructuredBuffer.h"

class GraphicsObject
{
public:
	GraphicsTopology m_Topology;
	std::vector<GraphicsMaterial*> m_Materials; // if m_Matarials.size() > 1 then GraphicsObject is batched || all meterials should be of the same type
	GraphicsMaterial* m_Material;
	//std::shared_ptr<MaterialBatchStructuredBuffer> m_MaterialsStructuredBuffer;

	GraphicsObject();
//	GraphicsObject() {}

	bool IsBatched() const;
	bool IsValid() const;

	bool m_Valid;
private:
	
};