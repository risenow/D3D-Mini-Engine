#include "stdafx.h"
#include <set>
#include "GraphicsObject.h"
#include "System/MemoryManager.h"
#include "Graphics/OrdinaryGraphicsObjectManager.h"
#include "Graphics/GraphicsMaterialsManager.h"

class GraphicObject;

size_t VertexDataStreamBase::GetNumVertexes() const
{
	return m_NumVertexes;

}
OrdinaryGraphicsObjectHandler::OrdinaryGraphicsObjectHandler() {}
OrdinaryGraphicsObjectHandler::OrdinaryGraphicsObjectHandler(const std::string& typeName) : m_TypeName(typeName) {}

bool OrdinaryGraphicsObjectHandler::CanHandle(tinyxml2::XMLElement* sceneGraphElement) const
{
	return sceneGraphElement->Name() == m_TypeName;
}

TriangleGraphicsObjectHandler::TriangleGraphicsObjectHandler() : OrdinaryGraphicsObjectHandler("triangle") {}
SerializableTriangleGraphicObject::SerializableTriangleGraphicObject(tinyxml2::XMLElement* element)
{
	Deserialize(element);
}
SerializableTriangleGraphicObject::VertexType  SerializableTriangleGraphicObject::ParseVertex(tinyxml2::XMLElement* vertexElement)
{
	return SerializableTriangleGraphicObject::VertexType({ glm::vec4(vertexElement->FloatAttribute("x"), vertexElement->FloatAttribute("y"), vertexElement->FloatAttribute("z"), 1.0f), vertexElement->UnsignedAttribute("uv", 0) });

}
void SerializableTriangleGraphicObject::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
{
	element->SetName(m_Type.c_str());
	for (size_t i = 0; i < m_MaterialNames.size(); i++)
		element->SetAttribute((std::string("material") + std::to_string(i)).c_str(), m_MaterialNames[i].c_str());

	for (const VertexType& vertexPosition : m_Vertexes)
	{
		tinyxml2::XMLElement* vertexElement = document.NewElement("vertex");
		vertexElement->SetAttribute("x", vertexPosition.m_Position.x);
		vertexElement->SetAttribute("y", vertexPosition.m_Position.y);
		vertexElement->SetAttribute("z", vertexPosition.m_Position.z);
		vertexElement->SetAttribute("uv", vertexPosition.m_TexCoord);
		element->InsertEndChild(vertexElement);
	}
}
void SerializableTriangleGraphicObject::Deserialize(tinyxml2::XMLElement* element)
{

	const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
	if (attr)
		while (attr)
		{
			std::string attrValue = std::string(attr->Value());
			m_MaterialNames.push_back(attrValue);
			attr = attr->Next();
		}

	tinyxml2::XMLElement* vertexElement = element->FirstChildElement();
	for (unsigned long i = 0; i < m_Vertexes.size(); i++)
	{
		assert(vertexElement);

		m_Vertexes[i] = ParseVertex(vertexElement);
		vertexElement = vertexElement->NextSiblingElement();
	}
}
TriangleGraphicsObjectHandler::HandleResult TriangleGraphicsObjectHandler::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const
{
	HandleResult result;
	std::string nm = std::string(sceneGraphElement->Name());
	if (std::string(sceneGraphElement->Name()) != m_TypeName)
		return result;

	SerializableTriangleGraphicObject* serializableTrangleGraphicObject = popNew(SerializableTriangleGraphicObject)(sceneGraphElement);
	result.m_SerializableGraphicObject = serializableTrangleGraphicObject;

	const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
	const std::string POSITION_PROPERTY_NAME = "POSITION";
	typedef glm::uint LocalTexCoordType ;

	VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });

	std::vector<std::string> attrs;
	size_t i = 0;
	const char* currentAtttrControl = 0;
	const char* currentAtttr = 0;

	while (currentAtttrControl = sceneGraphElement->Attribute((std::string("material") + std::to_string(i)).c_str(), currentAtttr))
	{ 
		result.m_GraphicsObjectSignature.m_Materials.push_back(materialsManager->GetMaterial(sceneGraphElement->Attribute((std::string("material") + std::to_string(i)).c_str())));
		i++;
	}

	result.m_GraphicsObjectSignature.m_SerializableGraphicsObject = serializableTrangleGraphicObject;
	result.m_GraphicsObjectSignature.m_VertexFormat = vertexFormat;
	result.m_GraphicsObjectSignature.m_XMLElement = sceneGraphElement;
	result.m_GraphicsObjectSignature.m_VertexDataStream = std::make_shared<VertexDataStream>(result.m_GraphicsObjectSignature);
	result.m_GraphicsObjectSignature.m_Valid = true;

	return result;
}

TriangleGraphicsObjectHandler::VertexDataStream::VertexDataStream()
{}
TriangleGraphicsObjectHandler::VertexDataStream::VertexDataStream(const OrdinaryGraphicsObjectSignature& signature) : m_Signature(signature)
{}

void TriangleGraphicsObjectHandler::VertexDataStream::Open()
{
	m_NumVertexes = SerializableTriangleGraphicObject::NumVertexes;
}
void TriangleGraphicsObjectHandler::VertexDataStream::Close()
{
}
void TriangleGraphicsObjectHandler::VertexDataStream::WriteTo(VertexData& data, size_t vertexesOffset)
{
	SerializableTriangleGraphicObject* serializableGraphicsObject = (SerializableTriangleGraphicObject*)m_Signature.m_SerializableGraphicsObject;
	
	memcpy((void*)((SerializableTriangleGraphicObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset), serializableGraphicsObject->m_Vertexes.data(), sizeof(SerializableTriangleGraphicObject::VertexType)*m_NumVertexes);
}


OrdinaryGraphicsObjectManager::OrdinaryGraphicsObjectManager() : m_Handlers({ popNew(TriangleGraphicsObjectHandler)() })
{
}
OrdinaryGraphicsObjectManager::~OrdinaryGraphicsObjectManager()
{
	for (OrdinaryGraphicsObjectHandler* handler : m_Handlers)
		if (handler)
			popDelete(handler);
}

GraphicsObjectManager::HandleResult OrdinaryGraphicsObjectManager::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	return HandleResult();
}


OrdinaryGraphicsObjectHandler::HandleResult OrdinaryGraphicsObjectManager::Handle_(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	for (OrdinaryGraphicsObjectHandler* handler : m_Handlers)
	{
		OrdinaryGraphicsObjectHandler::HandleResult handleResult = handler->Handle(device, shadersCollection, materialsManager, sceneGraphElement);
		if (handleResult.m_GraphicsObjectSignature.IsValid())
		{
			m_ObjectSignatures.push_back(handleResult.m_GraphicsObjectSignature);

			return OrdinaryGraphicsObjectHandler::HandleResult{ m_ObjectSignatures.back(), handleResult.m_SerializableGraphicObject };
		} 
	}
	return OrdinaryGraphicsObjectHandler::HandleResult();
}

std::map<unsigned int, std::shared_ptr<MaterialBatchStructuredBuffer>> MaterialBatchStructuredBuffers;

void OrdinaryGraphicsObjectManager::CompileGraphicObjects(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager)
{
    std::set<size_t> allreadyBatched;

    for (size_t m = 0; m < m_ObjectSignatures.size(); m++)
	{
        if (allreadyBatched.find(m) != allreadyBatched.end())
            continue;

		struct 
		{
			glm::uint begin;
			glm::uint end;
		} _range;

        std::vector<uint32_t> batchSet;

        size_t z = 1;
		for (size_t i = 0; i < m_ObjectSignatures.size(); i++)
		{
			if (m_ObjectSignatures[i].m_VertexFormat == m_ObjectSignatures[m].m_VertexFormat && m_ObjectSignatures[i].m_Materials.size() == m_ObjectSignatures[m].m_Materials.size())
			{
				bool match = true;
                for (size_t k = 0; k < m_ObjectSignatures[m].m_Materials.size(); k++)
                {
                    if (m_ObjectSignatures[m].m_Materials[k] != m_ObjectSignatures[i].m_Materials[k])
                        match = false;
                }
			
                if (match)
                {
                    batchSet.push_back(i);
                }
			}
		}

     
		size_t overallVertexesNum = 0;
		for (size_t i : batchSet)
		{
            m_ObjectSignatures[i].m_VertexDataStream->Open();
			overallVertexesNum += m_ObjectSignatures[i].m_VertexDataStream->GetNumVertexes();
		}


		size_t vertexesWritten = 0;
		VertexData vertexData(m_ObjectSignatures[m].m_VertexFormat, overallVertexesNum);
		for (size_t i : batchSet)
		{
			VertexFormat& vertexFormat = m_ObjectSignatures[i].m_VertexFormat;
			OrdinaryGraphicsObjectSignature& signature = m_ObjectSignatures[i];
			signature.m_VertexDataStream->WriteTo(vertexData, vertexesWritten);
			signature.m_VertexDataStream->Close();

			vertexesWritten += signature.m_VertexDataStream->GetNumVertexes();

            allreadyBatched.insert(i);
		}

			GraphicsObject object;
			object.m_Valid = true;
			object.m_Topology = GraphicsTopology(device, textureCollection, shadersCollection, vertexData, true);

			bool singleMaterial = false;
			if (m_ObjectSignatures[m].m_Materials.size() == 1)
			{
				object.m_Material = m_ObjectSignatures[m].m_Materials[0];
				singleMaterial = true;
			}

			if (!singleMaterial)
			{
				std::vector<GraphicsMaterial*> tempMaterials;
				for (unsigned int k = 0; k < m_ObjectSignatures[m].m_Materials.size(); k++)
					tempMaterials.push_back(m_ObjectSignatures[m].m_Materials[k]);

                object.m_Materials = tempMaterials;

                if (MaterialBatchStructuredBuffers.find(m) == MaterialBatchStructuredBuffers.end())
                {
                    MaterialBatchStructuredBuffers[m] = std::make_shared<MaterialBatchStructuredBuffer>(MaterialBatchStructuredBuffer(device, tempMaterials));
                    for (size_t i = 0; i < object.m_Materials.size(); i++)
                        object.m_Materials[i]->SetMaterialsStructuredBuffer(MaterialBatchStructuredBuffers[m]);
                }
                else
                {
                    for (size_t i = 0; i< object.m_Materials.size(); i++)
                        object.m_Materials[i]->SetMaterialsStructuredBuffer(MaterialBatchStructuredBuffers[m]);
                }
			}

			m_Objects.push_back(object);

	}

	size_t i = 0;
	for (OrdinaryGraphicsObjectSignature& signature : m_ObjectSignatures)
	{
		
	}
}

void OrdinaryGraphicsObjectManager::Render(GraphicsDevice& device, ShadersCollection& shadersCollection, const Camera& camera)
{
	for (GraphicsObject& object : m_Objects)
	{
        if (object.m_Material != nullptr)
            object.m_Material->Bind(device, shadersCollection);
        else
            object.m_Materials[0]->Bind(device, shadersCollection);

		object.m_Topology.Bind(device, object.m_Materials[0]->GetBuffer(), camera, Topology_Basic);

		object.m_Topology.DrawCall(device);
	}
}