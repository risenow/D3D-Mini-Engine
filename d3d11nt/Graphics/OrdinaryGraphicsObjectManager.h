#pragma once
#include <array>
#include <memory>
#include "System/Camera.h"
#include "GraphicsMaterial.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsObjectHandler.h"
#include "Graphics/GraphicsObjectManager.h"
#include "Graphics/SerializableGraphicsObject.h"
#include "Graphics/GraphicsTextureCollection.h"
#include "Graphics/LightingMaterial.h"
#include "Extern/tiny_obj_loader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/cimport.h>
#include <unordered_map>
#include <set>

class GraphicsObject;

//review the possibility of integrating entity system
//WriteTo implement!
class OrdinaryGraphicsObject : public SerializableGraphicsObject
{
public:
    OrdinaryGraphicsObject(bool tcEnabled = false, bool tbnEnabled = false, bool batched = false) : m_TexCoordEnabled(tcEnabled), m_TBNEnabled(tbnEnabled), m_Batchable(batched) {}
    virtual void SetTransform(const glm::mat4x4& t) const;

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched = false, size_t vertexesOffset = 0) = 0;
    virtual size_t GetNumVertexes() const = 0;

    bool TBNEnabled() { return m_TBNEnabled; }
    bool TexCoordEnabled() { return m_TexCoordEnabled; }

    virtual bool IsDrawable() { return true; }
public:
    GraphicsObject* m_Drawable;
    VertexFormat m_VertexFormat; //make batched and not versions
    VertexFormat m_BatchedVertexFormat;
    std::vector<GraphicsMaterial*> m_Materials;
    tinyxml2::XMLElement* m_XMLElement;
    bool m_Batchable;

    bool m_TBNEnabled;
    bool m_TexCoordEnabled;
};

class OrdinaryGraphicsObjectHandler;

class OrdinaryGraphicsObjectHandler
{
public:
    //expand to support submeshes
	struct HandleResult
	{
        std::vector<OrdinaryGraphicsObject*> m_OrdinaryGraphicsObjects;
	};

	OrdinaryGraphicsObjectHandler();
	OrdinaryGraphicsObjectHandler(const std::string& typeName);

	bool CanHandle(tinyxml2::XMLElement* sceneGraphElement) const;
	virtual HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const = 0;
protected:
	std::string m_TypeName;
};

class TriangleOrdinaryGraphicsObject : public OrdinaryGraphicsObject
{
    friend class TriangleGraphicsObjectHandler;
    friend class OrdinaryGraphicsObjectManager;
public:
    static const size_t NumVertexes = 3;
    typedef
        struct {
        glm::vec4 m_Position;
        glm::vec3 m_Normal;
    }
    VertexType;//need batched and non batched separate versions
    struct BatchedVertexType {
        BatchedVertexType(VertexType basic, glm::uint texCoord) : m_Position(basic.m_Position), m_Normal(basic.m_Normal), m_TexCoord(texCoord) {}
        glm::vec4 m_Position;
        glm::vec3 m_Normal;
        glm::uint m_TexCoord;
    };

    typedef glm::uvec2 TexCoordType;

    TriangleOrdinaryGraphicsObject(const std::array<VertexType, NumVertexes>& vertexes, const std::string& materialName, const std::string name);
    TriangleOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element);

    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
    void Deserialize(tinyxml2::XMLElement* element);

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched = false, size_t vertexesOffset = 0) override;
    virtual size_t GetNumVertexes() const override;
private:
    VertexData m_VertexData;
    VertexType ParseVertex(tinyxml2::XMLElement* vertexElement);

    std::array<TriangleOrdinaryGraphicsObject::VertexType, NumVertexes> m_Vertexes;
    std::vector<std::string> m_MaterialNames;
};

class TriangleGraphicsObjectHandler : public OrdinaryGraphicsObjectHandler
{
public:
	TriangleGraphicsObjectHandler();
	HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const override;
private:
};

class SphereOrdinaryGraphicsObject : public OrdinaryGraphicsObject
{
    friend class SphereGraphicsObjectHandler;
public:
    struct VertexType {

        VertexType(glm::vec4 pos, glm::vec3 normal, glm::uint texCoord) : m_Position(pos), m_Normal(normal){}
        glm::vec4 m_Position;
        glm::vec3 m_Normal;
    };
    struct BatchedVertexType {

        BatchedVertexType(VertexType basic, glm::uint texCoord) : m_Position(basic.m_Position), m_Normal(basic.m_Normal), m_TexCoord(texCoord) {}
        BatchedVertexType(glm::vec4 pos, glm::vec3 normal, glm::uint texCoord) : m_Position(pos), m_Normal(normal), m_TexCoord(texCoord) {}
        glm::vec4 m_Position;
        glm::vec3 m_Normal;
        glm::uint m_TexCoord;
    };
    typedef glm::uvec2 TexCoordType;

    SphereOrdinaryGraphicsObject(glm::vec3 center, float r, const std::string& materialName, const std::string name);
    SphereOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element);

    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
    void Deserialize(tinyxml2::XMLElement* element);

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched = false, size_t vertexesOffset = 0) override;
    virtual size_t GetNumVertexes() const override;

private:
    glm::vec3 m_Center;
    float m_R;
    glm::uint m_MatX;
    std::vector<std::string> m_MaterialNames;

    std::vector<VertexType> m_Vertexes;
    std::vector<uint32_t> m_Indexes;
};

class SphereGraphicsObjectHandler : public OrdinaryGraphicsObjectHandler
{
public:
    SphereGraphicsObjectHandler();
    HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const override;
private:
};

class MetaModelGraphicsObject : public OrdinaryGraphicsObject
{
public:
    friend class SubMeshModelGraphicsObject;

    MetaModelGraphicsObject(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element);

    void ProduceSubMeshes(std::vector<OrdinaryGraphicsObject*>& graphicsObjects);

    //to implement
    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) {}
    void Deserialize(tinyxml2::XMLElement* element) 
    {
        const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
        popAssert(attr);
        if (attr)
        {
            if (!strcmp(attr->Name(), "path"))
            {
                m_Path = attr->Value();
            }
        }
    }

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched = false, size_t vertexesOffset = 0) override {}
    virtual size_t GetNumVertexes() const override { return 0; }
    
    virtual bool IsDrawable() { return false; }

protected:
    std::string m_Path;
    Assimp::Importer            importer;
    const aiScene* m_ModelScene;
};

bool operator<(const tinyobj::index_t& v1, const tinyobj::index_t& v2);
struct ObjIndexComp
{
    bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const 
    {
        return a < b;
    }
};

class SubMeshModelGraphicsObject : public OrdinaryGraphicsObject
{
public:
    typedef glm::vec4 PositionType;
    typedef glm::vec3 NormalType;
    typedef glm::vec2 TcType;
    struct VertexType {

        VertexType(const glm::vec4& pos, const glm::vec3& normal, const glm::vec2& texCoord) : m_Position(pos), m_Normal(normal), m_TexCoord(texCoord) {}
        glm::vec4 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_TexCoord;
    };

    SubMeshModelGraphicsObject(MetaModelGraphicsObject* metaObj, size_t shapeIndex);

    //not to implement
    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) {}
    void Deserialize(tinyxml2::XMLElement* element) {}

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched = false, size_t vertexesOffset = 0) override;
    virtual size_t GetNumVertexes() const override;

private:
    MetaModelGraphicsObject* m_MetaObj;
    size_t m_ShapeIndex;
};

class ModelGraphicsObjectHandler : public OrdinaryGraphicsObjectHandler
{
public:
    ModelGraphicsObjectHandler() : OrdinaryGraphicsObjectHandler("model") {}
    HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const override
    {
        HandleResult result;
        result.m_OrdinaryGraphicsObjects = { };
        std::string nm = std::string(sceneGraphElement->Name());
        if (std::string(sceneGraphElement->Name()) != m_TypeName)
            return result;

        MetaModelGraphicsObject* metaObjModleGraphicsObject = popNew(MetaModelGraphicsObject)(device, shadersCollection, textureCollection, materialsManager, sceneGraphElement);
        result.m_OrdinaryGraphicsObjects.push_back(metaObjModleGraphicsObject);
        metaObjModleGraphicsObject->ProduceSubMeshes(result.m_OrdinaryGraphicsObjects);

        return result;
    }

private:
};

class OrdinaryGraphicsObjectManager : public GraphicsObjectManager
{
public:
	OrdinaryGraphicsObjectManager();
	~OrdinaryGraphicsObjectManager();

	void Render(GraphicsDevice& device, ShadersCollection& shaderCollection, uint32_t passShaderBits, const Camera& camera, bool shadowPass = false);

	GraphicsObjectManager::HandleResult Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);
	OrdinaryGraphicsObjectHandler::HandleResult Handle_(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);

    //run it in separate thread
	void CompileGraphicObjects(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager);
private:
    std::vector<OrdinaryGraphicsObject*> m_Objects;

	std::vector<GraphicsObject> m_DrawableObjects;
	std::vector<OrdinaryGraphicsObjectHandler*> m_Handlers;
};