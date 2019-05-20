#pragma once
#include <array>
#include <memory>
#include "System/Camera.h"
#include "GraphicsMaterial.h";
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/GraphicsObjectHandler.h"
#include "Graphics/GraphicsObjectManager.h"
#include "Graphics/SerializableGraphicsObject.h"
#include "Graphics/GraphicsTextureCollection.h"

class GraphicsObject;

class VertexDataStreamBase
{
public:
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual void WriteTo(VertexData& data, size_t vertexesOffset = 0) = 0;

	size_t GetNumVertexes() const;
protected:
	size_t m_NumVertexes;
};

class OrdinaryGraphicsObjectHandler;
class OrdinaryGraphicsObjectSignature
{
public:
	OrdinaryGraphicsObjectSignature() : m_Valid(false) {}

	bool IsValid() const { return m_Valid; }

	VertexFormat m_VertexFormat;
	std::vector<GraphicsMaterial*> m_Materials;
	tinyxml2::XMLElement* m_XMLElement;
	std::shared_ptr<VertexDataStreamBase> m_VertexDataStream;
	SerializableGraphicsObject* m_SerializableGraphicsObject;

	bool m_Valid;
private:
};

class OrdinaryGraphicsObjectHandler
{
public:
	struct HandleResult
	{
		OrdinaryGraphicsObjectSignature m_GraphicsObjectSignature;
		SerializableGraphicsObject* m_SerializableGraphicObject;
	};

	OrdinaryGraphicsObjectHandler();
	OrdinaryGraphicsObjectHandler(const std::string& typeName);

	bool CanHandle(tinyxml2::XMLElement* sceneGraphElement) const;
	virtual HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const = 0;
	//virtual std::shared_ptr<VertexDataStreamBase> GetVertexDataStream() = 0;
protected:
	std::string m_TypeName;
};

class SerializableTriangleGraphicObject : public SerializableGraphicsObject
{	
	friend class TriangleGraphicsObjectHandler;
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

	SerializableTriangleGraphicObject(const std::array<VertexType, NumVertexes>& vertexes, const std::string& materialName, const std::string name);
	SerializableTriangleGraphicObject(tinyxml2::XMLElement* element);

	void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
	void Deserialize(tinyxml2::XMLElement* element);

private:
	VertexType ParseVertex(tinyxml2::XMLElement* vertexElement);

	std::array<SerializableTriangleGraphicObject::VertexType, NumVertexes> m_Vertexes;
	std::vector<std::string> m_MaterialNames;
};

class TriangleGraphicsObjectHandler : public OrdinaryGraphicsObjectHandler
{
public:
	TriangleGraphicsObjectHandler();
	HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const;

	class VertexDataStream : public VertexDataStreamBase
	{
	public:
		VertexDataStream();
		VertexDataStream(const OrdinaryGraphicsObjectSignature& signature);

		virtual void Open();
		virtual void Close();
		virtual void WriteTo(VertexData& data, size_t vertexesOffset = 0);
	private:
		OrdinaryGraphicsObjectSignature m_Signature;
	};

	//virtual std::shared_ptr<VertexDataStreamBase> GetVertexDataStream() { return std::make_shared<VertexDataStream>(); }
private:
};

class SerializableSphereGraphicObject : public SerializableGraphicsObject
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

    SerializableSphereGraphicObject(glm::vec3 center, float r, const std::string& materialName, const std::string name);
    SerializableSphereGraphicObject(tinyxml2::XMLElement* element);

    void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document);
    void Deserialize(tinyxml2::XMLElement* element);

private:
    glm::vec3 m_Center;
    float m_R;
    std::vector<std::string> m_MaterialNames;
};

class SphereGraphicsObjectHandler : public OrdinaryGraphicsObjectHandler
{
public:
    SphereGraphicsObjectHandler();
    HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const;

    class VertexDataStream : public VertexDataStreamBase
    {
    public:
        VertexDataStream();
        VertexDataStream(const OrdinaryGraphicsObjectSignature& signature, glm::vec3 center, float r);

        virtual void Open();
        virtual void Close();
        virtual void WriteTo(VertexData& data, size_t vertexesOffset = 0);
    private:
        OrdinaryGraphicsObjectSignature m_Signature;
        glm::vec3 m_Center;
        float m_R;
        std::vector<SerializableSphereGraphicObject::VertexType> m_Vertexes;
    };

    //virtual std::shared_ptr<VertexDataStreamBase> GetVertexDataStream() { return std::make_shared<VertexDataStream>(); }
private:
};

class OrdinaryGraphicsObjectManager : public GraphicsObjectManager
{
public:
	OrdinaryGraphicsObjectManager();
	~OrdinaryGraphicsObjectManager();

	void Render(GraphicsDevice& device, ShadersCollection& shaderCollection, const Camera& camera);

	GraphicsObjectManager::HandleResult Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);
	OrdinaryGraphicsObjectHandler::HandleResult Handle_(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);

	void CompileGraphicObjects(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager);
private:
	std::vector<OrdinaryGraphicsObjectSignature> m_ObjectSignatures;
	std::vector<GraphicsObject> m_Objects;
	std::vector<OrdinaryGraphicsObjectHandler*> m_Handlers;
};