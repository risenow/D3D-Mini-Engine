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
#include <set>

class GraphicsObject;

//review the possibility of integrating entity system
//WriteTo implement!
class OrdinaryGraphicsObject : public SerializableGraphicsObject
{
public:
    virtual void SetTransform(const glm::mat4x4& t) const;

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, size_t vertexesOffset = 0) = 0;
    virtual size_t GetNumVertexes() const = 0;
public:
    GraphicsObject* m_Drawable;
    VertexFormat m_VertexFormat;
    std::vector<GraphicsMaterial*> m_Materials;
    tinyxml2::XMLElement* m_XMLElement;
};

class OrdinaryGraphicsObjectHandler;

class OrdinaryGraphicsObjectHandler
{
public:
	struct HandleResult
	{
        OrdinaryGraphicsObject* m_OrdinaryGraphicsObject;
	};

	OrdinaryGraphicsObjectHandler();
	OrdinaryGraphicsObjectHandler(const std::string& typeName);

	bool CanHandle(tinyxml2::XMLElement* sceneGraphElement) const;
	virtual HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const = 0;
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
        glm::uint m_TexCoord;
    }
    VertexType;
    typedef glm::uvec2 TexCoordType;

    TriangleOrdinaryGraphicsObject(const std::array<VertexType, NumVertexes>& vertexes, const std::string& materialName, const std::string name);
    TriangleOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element);

    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
    void Deserialize(tinyxml2::XMLElement* element);

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, size_t vertexesOffset = 0) override;
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
	HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const;
private:
};

class SphereOrdinaryGraphicsObject : public OrdinaryGraphicsObject
{
    friend class SphereGraphicsObjectHandler;
public:
    struct VertexType {

        VertexType(glm::vec4 pos, glm::vec3 normal, glm::uint texCoord) : m_Position(pos), m_Normal(normal), m_TexCoord(texCoord) {}
        glm::vec4 m_Position;
        glm::vec3 m_Normal;
        glm::uint m_TexCoord;
    };
    typedef glm::uvec2 TexCoordType;

    SphereOrdinaryGraphicsObject(glm::vec3 center, float r, const std::string& materialName, const std::string name);
    SphereOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element);

    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
    void Deserialize(tinyxml2::XMLElement* element);

    virtual void WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, size_t vertexesOffset = 0) override;
    virtual size_t GetNumVertexes() const override;
private:
    glm::vec3 m_Center;
    float m_R;
    glm::uint m_MatX;
    std::vector<std::string> m_MaterialNames;

    std::vector<VertexType> m_Vertexes;
};

class SphereGraphicsObjectHandler : public OrdinaryGraphicsObjectHandler
{
public:
    SphereGraphicsObjectHandler();
    HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const;
private:
};

class OrdinaryGraphicsObjectManager : public GraphicsObjectManager
{
public:
	OrdinaryGraphicsObjectManager();
	~OrdinaryGraphicsObjectManager();

	void Render(GraphicsDevice& device, ShadersCollection& shaderCollection, const std::vector<GraphicsShaderMacro>& passMacros, const Camera& camera);

	GraphicsObjectManager::HandleResult Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);
	OrdinaryGraphicsObjectHandler::HandleResult Handle_(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);

    //run it in separate thread
	void CompileGraphicObjects(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager);
private:
    std::vector<OrdinaryGraphicsObject*> m_Objects;

	std::vector<GraphicsObject> m_DrawableObjects;
	std::vector<OrdinaryGraphicsObjectHandler*> m_Handlers;
};