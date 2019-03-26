#pragma once
#include <string>
#include "Extern/tinyxml2.h"
#include "Graphics/GraphicsMaterial.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/ShadersCollection.h"
#include "Data/Shaders/Test/psconstants.h"

class GraphicsPlainColorMaterial : public GraphicsMaterial
{
public:
	GraphicsPlainColorMaterial();
	GraphicsPlainColorMaterial(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::string& name, float redFactor);
	GraphicsPlainColorMaterial(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::string& name, const std::vector<GraphicsMaterial*> batchedMaterials);

	bool HasValidConstantsBuffer() const;
	GraphicsBuffer* GetConstantsBuffer() const;
	void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, size_t variationIndex = 0);

	virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
	virtual void Deserialize(tinyxml2::XMLElement* element);

	virtual void* GetDataPtr();
	virtual size_t GetDataSize();

	static GraphicsMaterial* Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement);
private:
	PSConsts m_Data;
	ShaderID m_ShaderID;

	static GraphicsConstantsBuffer<PSConsts> m_ConstantsBuffer;
	static bool m_ConstantsBufferInitialized;
};