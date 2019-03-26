#pragma once
#include <string>
#include "Extern/tinyxml2.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/SerializableGraphicsObject.h"

class MaterialBatchStructuredBuffer;

class GraphicsMaterial : public SerializableGraphicsObject
{
public:
	virtual bool HasValidConstantsBuffer() const = 0;
	virtual GraphicsBuffer* GetConstantsBuffer() const = 0;
	virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, size_t variationIndex = 0) = 0;

	virtual void* GetDataPtr() = 0;
	virtual size_t GetDataSize() = 0;

	std::string GetName() const;

	bool IsBatched();
protected:
	std::string m_Name;
	GraphicsPixelShader m_Shader; // to remove
	//std::vector<ShaderVariation> m_ShaderVariations;
	std::vector<ShaderID> m_ShaderVariationIDs;
	
static	MaterialBatchStructuredBuffer m_MaterialBatchStructuredBuffer;
};

typedef GraphicsMaterial*(*GraphicsMaterialHandleFunc)(GraphicsDevice&, ShadersCollection&, tinyxml2::XMLElement*);