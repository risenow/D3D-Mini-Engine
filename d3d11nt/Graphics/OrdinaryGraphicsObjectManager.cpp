#include "stdafx.h"
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


	//const char* nameAttr = element->Attribute("material");
	//assert(nameAttr);
	//m_MaterialName = nameAttr;

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
	else
		bool sh = true;

	/*const std::string POSITION_PROPERTY_NAME = "POSITION";

	VertexFormat vertexFormat;

	vertexFormat.AddVertexProperty(CreateVertexPropertyPrototype<PositionType>(POSITION_PROPERTY_NAME));

	const count_t VERTEX_COUNT = 3;
	VertexData vertexData(vertexFormat, VERTEX_COUNT);
	VertexProperty positionProperty = vertexData.GetVertexPropertyByName(POSITION_PROPERTY_NAME);

	tinyxml2::XMLElement* vertexElement = sceneGraphElement->FirstChildElement();
	for (unsigned long i = 0; i < VERTEX_COUNT; i++)
	{
		(*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(i, positionProperty)) = ParseVertex(vertexElement);
		vertexElement = vertexElement->NextSiblingElement();
	}*/

	/*std::array<SerializableTriangleGraphicObject::PositionType, 3> vertexes;
	tinyxml2::XMLElement* vertexElement = sceneGraphElement->FirstChildElement();
	for (unsigned long i = 0; i < vertexes.szie(); i++)
	{
		(*(PositionType*)vertexData.GetVertexPropertyDataPtrForVertexWithIndex(i, positionProperty)) = ParseVertex(vertexElement);
		vertexElement = vertexElement->NextSiblingElement();
	}*/

	SerializableTriangleGraphicObject* serializableTrangleGraphicObject = popNew(SerializableTriangleGraphicObject)(sceneGraphElement);
	result.m_SerializableGraphicObject = serializableTrangleGraphicObject;
	/*
	const std::string POSITION_PROPERTY_NAME = "POSITION";
	const std::string MATERIAL_PROPERTY_NAME = "TEXCOORD";
	VertexFormat vertexFormat({ CreateVertexPropertyPrototype<SerializableTriangleGraphicObject::PositionType>(POSITION_PROPERTY_NAME) , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1)});
	VertexData vertexData(vertexFormat, serializableTrangleGraphicObject->m_Vertexes.size());
	VertexProperty positionProperty = vertexData.GetVertexPropertyByName(POSITION_PROPERTY_NAME);
	VertexProperty materialProperty = vertexData.GetVertexPropertyByName(MATERIAL_PROPERTY_NAME);

	unsigned long k = 0;
	SerializableTriangleGraphicObject::PositionType* vertexPositionPropertyDataForVertex = (SerializableTriangleGraphicObject::PositionType*)vertexData.GetVertexPropertyDataPtr(positionProperty);
	uint32_t* vertexMaterialPropertyDataForVertex = (uint32_t*)vertexData.GetVertexPropertyDataPtr(materialProperty);
	for (SerializableTriangleGraphicObject::PositionType& triangleVertexPosition : serializableTrangleGraphicObject->m_Vertexes)
	{
		(*(vertexPositionPropertyDataForVertex++)) = triangleVertexPosition;
		(*vertexMaterialPropertyDataForVertex) = k;
		k++;
	}


	result.m_GraphicObject.m_Topology = GraphicsTopology(device, shadersCollection, vertexData);
	result.m_GraphicObject.m_Material = materialsManager->GetMaterial(sceneGraphElement->Attribute("material"));
	result.m_GraphicObject.m_Valid = true;*/

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
		//const std::string currentAtttrValue = currentAtttr;
		//int i = std::stol(currentAtttrValue);

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

void OrdinaryGraphicsObjectManager::CompileGraphicObjects(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager)
{
	std::vector<size_t> batchedObjectsIndexes;
	while (m_ObjectSignatures.size())
	{

		struct 
		{
			glm::uint begin;
			glm::uint end;
		} _range;
		batchedObjectsIndexes.resize(0);
        size_t z = 1;
		for (size_t i = 0; i < m_ObjectSignatures.size() - 1; i++)
		{
			if (m_ObjectSignatures[i].m_VertexFormat == m_ObjectSignatures[0].m_VertexFormat)
			{
				bool match = false;
                for (size_t k = 0; k < m_ObjectSignatures[i].m_Materials.size(); k++)
                {
                    for (size_t j = k + 1; j < m_ObjectSignatures[i + 1].m_Materials.size(); j++)
                        if (m_ObjectSignatures[i].m_Materials[k] != m_ObjectSignatures[i + z].m_Materials[k])
                            match = true;
                    if (!match)
                    {
                        batchedObjectsIndexes.push_back(i);
                        continue;
                    }
                    z++;
                }
			
				batchedObjectsIndexes.push_back(i);
			}
		}

		size_t overallVertexesNum = 0;
		for (size_t i : batchedObjectsIndexes)
		{
			overallVertexesNum += m_ObjectSignatures[i].m_VertexDataStream->GetNumVertexes();
		}

		//typedef UVType glm::vec2;
		//vertexFormat.AddVertexProperty(CreateVertexPropertyPrototype<PositionType>(POSITION_PROPERTY_NAME));

		size_t vertexesWritten = 0;
		VertexData vertexData(m_ObjectSignatures[0].m_VertexFormat, overallVertexesNum);
		for (size_t i : batchedObjectsIndexes)
		{
			VertexFormat& vertexFormat = m_ObjectSignatures[i].m_VertexFormat;
			//vertexFormat.AddVertexProperty(CreateVertexPropertyPrototype<PositionType>(POSITION_PROPERTY_NAME));
			OrdinaryGraphicsObjectSignature& signature = m_ObjectSignatures[i];
			signature.m_VertexDataStream->Open();
			signature.m_VertexDataStream->WriteTo(vertexData, vertexesWritten);
			signature.m_VertexDataStream->Close();

			vertexesWritten += signature.m_VertexDataStream->GetNumVertexes();
		}

		std::vector<GraphicsMaterial*> batchedMaterials;
		for (size_t i : batchedObjectsIndexes)
		{
			batchedMaterials.push_back(m_ObjectSignatures[i].m_Materials[i]);

			std::shared_ptr<VertexDataStreamBase> vertexDataStream = m_ObjectSignatures[i].m_VertexDataStream;
			vertexDataStream->Open();
			VertexData vertexData(m_ObjectSignatures[i].m_VertexFormat, vertexDataStream->GetNumVertexes());
			vertexDataStream->WriteTo(vertexData);
			vertexDataStream->Close();

			GraphicsObject object;
			object.m_Valid = true;
			object.m_Topology = GraphicsTopology(device, shadersCollection, vertexData);

			bool singleMaterial = false;
			if (m_ObjectSignatures[i].m_Materials.size() == 1)
			{
				object.m_Material = m_ObjectSignatures[i].m_Materials[0];
				singleMaterial = true;
			}

			if (!singleMaterial)
			{
				std::vector<GraphicsMaterial*> tempMaterials;
				for (unsigned int k = 0; k < m_ObjectSignatures[i].m_Materials.size(); k++)
					tempMaterials[k] = m_ObjectSignatures[i].m_Materials[k];

				if (MaterialBatchStructuredBuffers.find(i) == MaterialBatchStructuredBuffers.end())
					MaterialBatchStructuredBuffers[i] = std::make_shared<MaterialBatchStructuredBuffer>(MaterialBatchStructuredBuffer(device, tempMaterials));
				else
					object.m_MaterialsStructuredBuffer = MaterialBatchStructuredBuffers[i];
				//object.m_MaterialsStructuredBuffer = MaterialBatchStructuredBuffer(device, tempMaterials);
			}

			m_Objects.push_back(object);

			i++;
		}
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
		object.m_Topology.Bind(device, camera);
		object.m_Material->Bind(device, shadersCollection);
		//BindMultipleGraphicsConstantBuffers(device, 0, { object.m_Topology.GetConstantsBuffer()  },  GraphicsShaderMask_Vertex );
		//BindMultipleGraphicsConstantBuffers(device, 0, { object.m_Material->GetConstantsBuffer() }, GraphicsShaderMask_Pixel  );

		object.m_Topology.DrawCall(device);
	}
}