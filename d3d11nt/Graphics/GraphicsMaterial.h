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
    virtual bool HasValidConstantsBuffer() const = 0;
    virtual GraphicsBuffer* GetConstantsBuffer() const = 0;
	virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex = 0) = 0;

    virtual void* GetDataPtr() = 0;
    virtual size_t GetDataSize() = 0;

	std::string GetName() const;

    void SetMaterialsStructuredBuffer(std::shared_ptr<MaterialBatchStructuredBuffer> materialStructuredBuffer);
    GraphicsBuffer& GetBuffer();

    void Update(const std::vector<GraphicsLightObject>& lights);

	bool IsBatched();

    std::vector<GraphicsLightObject>& GetLights() { return m_Lights; }
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
    TypedGraphicsMaterial() {}
    TypedGraphicsMaterial(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, const std::string& name) : m_ShaderStrIdentifier(shaderStrIdentifier)
    {
        m_Name = name;
        if (!m_ConstantsBufferInitialized)
        {
            m_ConstantsBuffer = GraphicsConstantsBuffer<T>(device);
            m_ConstantsBufferInitialized = true;
        }
    }
    virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, const std::vector<GraphicsShaderMacro>& passMacros, size_t variationIndex = 0) override
    {
        m_Shader = shadersCollection.GetShader<GraphicsPixelShader>(m_ShaderStrIdentifier.path, m_ShaderStrIdentifier.variation);
        m_Shader.Bind(device);

        if (m_ConstantsBufferInitialized && consts)
        {
            m_Data = *((T*)consts);
            m_ConstantsBuffer.Update(device, m_Data);
            BindMultipleGraphicsConstantBuffers(device, 0, { &m_ConstantsBuffer }, GraphicsShaderMask_Pixel);
        }
    }
    virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) override {}
    virtual void Deserialize(tinyxml2::XMLElement* element) override {}


    virtual void* TypedGraphicsMaterial::GetDataPtr() override
    {
        return &m_Data;
    }
    virtual size_t TypedGraphicsMaterial::GetDataSize() override
    {
        return sizeof(T);
    }

    virtual bool HasValidConstantsBuffer() const override
    {
        return m_ConstantsBuffer.GetBuffer() != nullptr;
    }
    virtual GraphicsBuffer*GetConstantsBuffer() const override
    {
        return (GraphicsBuffer*)& m_ConstantsBuffer;
    }

protected:
    T m_Data;
    ShaderStrIdentifier m_ShaderStrIdentifier;

    static GraphicsConstantsBuffer<T> m_ConstantsBuffer;
    static bool m_ConstantsBufferInitialized;
};

template<class T>
GraphicsConstantsBuffer<T> TypedGraphicsMaterial<T>::m_ConstantsBuffer;
template<class T>
bool TypedGraphicsMaterial<T>::m_ConstantsBufferInitialized;
