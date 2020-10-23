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
#include "Graphics/MaterialBatchStructuredBuffer.h"
#include "System/Camera.h"

class MaterialBatchStructuredBuffer;

class GraphicsMaterial : public SerializableGraphicsObject
{
public:
    virtual bool HasValidConstantsBuffer() const = 0;
    virtual GraphicsBuffer* GetConstantsBuffer() const = 0;
	virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, uint32_t shaderBits, size_t variationIndex = 0) = 0;

    virtual void* GetDataPtr() = 0;
    virtual size_t GetDataSize() = 0;

	std::string GetName() const;

    void SetMaterialsStructuredBuffer(std::shared_ptr<MaterialBatchStructuredBuffer> materialStructuredBuffer);
    GraphicsBuffer& GetBuffer();

    void Update(const std::vector<GraphicsLightObject>& lights);

	bool IsBatched();

    void FillMaterialBits(uint32_t& dst, const uint32_t passMacros);

    static void BindNull(GraphicsDevice& device)
    {
        device.GetD3D11DeviceContext()->PSSetShader(nullptr, nullptr, 0);
    }

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
class TypedGraphicsMaterialBase : public GraphicsMaterial
{
public:
    TypedGraphicsMaterialBase() {}
    TypedGraphicsMaterialBase(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, const std::string& name) : m_ShaderStrIdentifier(shaderStrIdentifier)
    {
        m_Name = name;
        if (!m_ConstantsBufferInitialized)
        {
            m_ConstantsBuffer = GraphicsConstantsBuffer<T>(device);
            m_ConstantsBufferInitialized = true;
        }
    }

    virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) override {}
    virtual void Deserialize(tinyxml2::XMLElement* element) override {}

    virtual void* GetDataPtr() override
    {
        return &m_Data;
    }
    virtual size_t GetDataSize() override
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
    bool _Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, uint32_t shaderBits)
    {
        uint32_t macros;
        FillMaterialBits(macros, shaderBits);

        m_Shader = shadersCollection.GetShader<GraphicsPixelShader>(m_ShaderStrIdentifier.path, macros);
        m_Shader.Bind(device);

        if (m_MaterialStructuredBuffer)
        {
            m_MaterialStructuredBuffer->Bind(device);
            return false;
        }

        if (m_ConstantsBufferInitialized)
        {
            m_ConstantsBuffer.Update(device, m_Data);
            BindMultipleGraphicsConstantBuffers(device, 0, { &m_ConstantsBuffer }, GraphicsShaderMask_Pixel);
        }

        return true;
    }

    T m_Data;
    ShaderStrIdentifier m_ShaderStrIdentifier;

    static GraphicsConstantsBuffer<T> m_ConstantsBuffer;
    static bool m_ConstantsBufferInitialized;
};

template<class T>
GraphicsConstantsBuffer<T> TypedGraphicsMaterialBase<T>::m_ConstantsBuffer;
template<class T>
bool TypedGraphicsMaterialBase<T>::m_ConstantsBufferInitialized;

template<class T>
class BasicGraphicsMaterial : public TypedGraphicsMaterialBase<T>
{
public:
    BasicGraphicsMaterial() {}
    BasicGraphicsMaterial(GraphicsDevice& device, ShadersCollection& shadersCollection, const ShaderStrIdentifier& shaderStrIdentifier, const std::string& name) : TypedGraphicsMaterialBase<T>(device, shadersCollection, shaderStrIdentifier, name)
    {}

    virtual void Bind(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera, void* consts, uint32_t passBits, size_t variationIndex = 0) override
    {
        if (consts)
            m_Data = *((T*)consts);

        _Bind(device, shadersCollection, passBits);
    }
protected:
};
