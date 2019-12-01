#pragma once
#include <string>
#include "Extern/tinyxml2.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsShader.h"
#include "Graphics/GraphicsConstantsBuffer.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsLightObject.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "Graphics/SerializableGraphicsObject.h"
#include "System/Camera.h"

class MaterialBatchStructuredBuffer;

class GraphicsMaterial : public SerializableGraphicsObject
{
public:
    virtual bool HasValidConstantsBuffer() const { return false; };
    virtual GraphicsBuffer* GetConstantsBuffer() const { return nullptr; };
	virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex = 0) = 0;

    virtual void* GetDataPtr()= 0;// {}
    virtual size_t GetDataSize() = 0;// {}

	std::string GetName() const;

    void SetMaterialsStructuredBuffer(std::shared_ptr<MaterialBatchStructuredBuffer> materialStructuredBuffer);
    GraphicsBuffer& GetBuffer();

    void Update(const std::vector<GraphicsLightObject>& lights);

	bool IsBatched();
protected:
	std::string m_Name;
	GraphicsPixelShader m_Shader; // to remove
	std::vector<ShaderID> m_ShaderVariationIDs;
    std::shared_ptr< MaterialBatchStructuredBuffer> m_MaterialStructuredBuffer;
    std::vector<GraphicsLightObject> m_Lights;
};

typedef GraphicsMaterial*(*GraphicsMaterialHandleFunc)(GraphicsDevice&, GraphicsTextureCollection&, ShadersCollection&, tinyxml2::XMLElement*);

template<class T>
class TypedGraphicsMaterial : public GraphicsMaterial
{
public:
    virtual void* TypedGraphicsMaterial::GetDataPtr() override
    {
        return &m_Data;
    }
    virtual size_t TypedGraphicsMaterial::GetDataSize() override
    {
        return sizeof(T);
    }

    bool HasValidConstantsBuffer() const
    {
        return m_ConstantsBuffer.GetBuffer() != nullptr;
    }
    GraphicsBuffer*GetConstantsBuffer() const
    {
        return (GraphicsBuffer*)& m_ConstantsBuffer;
    }

protected:
    T m_Data;

    static GraphicsConstantsBuffer<T> m_ConstantsBuffer;
    static bool m_ConstantsBufferInitialized;
};