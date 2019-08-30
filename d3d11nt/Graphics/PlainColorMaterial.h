#pragma once
#include <string>
#include "Extern/tinyxml2.h"
#include "Graphics/GraphicsMaterial.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsTextureCollection.h"
#include <SimpleMath.h>
using namespace DirectX;
#include "Data/Shaders/Test/psconstants.h"

class GraphicsPlainColorMaterial : public GraphicsMaterial
{
public:
	GraphicsPlainColorMaterial();
    GraphicsPlainColorMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, tinyxml2::XMLElement* element);//float redFactor);
	GraphicsPlainColorMaterial(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& name, const std::vector<GraphicsMaterial*> batchedMaterials);

	bool HasValidConstantsBuffer() const;
	GraphicsBuffer* GetConstantsBuffer() const;
	void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex = 0);

	virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
	virtual void Deserialize(tinyxml2::XMLElement* element);

	virtual void* GetDataPtr();
	virtual size_t GetDataSize();

	static GraphicsMaterial* Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, tinyxml2::XMLElement* sceneGraphElement);
private:
	PSConsts m_Data;
	ShaderID m_ShaderID;

    ID3D11SamplerState* m_SamplerState;

    std::shared_ptr<Texture2D> m_Cubemap;

	static GraphicsConstantsBuffer<PSConsts> m_ConstantsBuffer;
	static bool m_ConstantsBufferInitialized;
};