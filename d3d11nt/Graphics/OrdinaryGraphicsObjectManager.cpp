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

glm::vec4 ParsePos(const std::string& pos)
{
    std::string temp;
    temp.reserve(6);
    glm::vec4 result;
    size_t compx = 0;
    for (size_t i = 0; i < pos.size(); i++)
    {
        if (pos[i] != ' ')
            temp.push_back(pos[i]);
        if (pos[i] == ' ' || i == pos.size()-1 )
        {
            result[compx] = std::stof(temp);
            compx++;
            temp = "";
        }
    }
    if (compx != 4)
        result[compx] = 1.0;

    return result;
}

std::string MakePosStr(const glm::vec3& pos)
{
    std::string p;
    p.reserve(18);
    p = std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z);
    return p;
}

TriangleGraphicsObjectHandler::TriangleGraphicsObjectHandler() : OrdinaryGraphicsObjectHandler("triangle") {}
SerializableTriangleGraphicObject::SerializableTriangleGraphicObject(tinyxml2::XMLElement* element)
{
	Deserialize(element);
}
SerializableTriangleGraphicObject::VertexType  SerializableTriangleGraphicObject::ParseVertex(tinyxml2::XMLElement* vertexElement)
{
	return SerializableTriangleGraphicObject::VertexType({ ParsePos(std::string(vertexElement->Attribute("p"))), glm::vec3(ParsePos(std::string(vertexElement->Attribute("n")))), vertexElement->UnsignedAttribute("uv", 0) });

}
void SerializableTriangleGraphicObject::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
{
	element->SetName(m_Type.c_str());
	for (size_t i = 0; i < m_MaterialNames.size(); i++)
		element->SetAttribute((std::string("material") + std::to_string(i)).c_str(), m_MaterialNames[i].c_str());

	for (const VertexType& vertexPosition : m_Vertexes)
	{
		tinyxml2::XMLElement* vertexElement = document.NewElement("vertex");
        
        vertexElement->SetAttribute("p", MakePosStr(glm::vec3(vertexPosition.m_Position)).c_str());
        vertexElement->SetAttribute("n", MakePosStr(vertexPosition.m_Normal).c_str());
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
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
	const std::string POSITION_PROPERTY_NAME = "POSITION";
	typedef glm::uint LocalTexCoordType ;

	VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });

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

SerializableSphereGraphicObject::SerializableSphereGraphicObject(tinyxml2::XMLElement* element)
{
    Deserialize(element);
}

void SerializableSphereGraphicObject::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
{
    element->SetName(m_Type.c_str());
    for (size_t i = 0; i < m_MaterialNames.size(); i++)
        element->SetAttribute((std::string("material") + std::to_string(i)).c_str(), m_MaterialNames[i].c_str());

    tinyxml2::XMLElement* centerElement = document.NewElement("center");
    centerElement->SetAttribute("p", MakePosStr(m_Center).c_str());
    element->InsertEndChild(centerElement);

    tinyxml2::XMLElement* radiusElement = document.NewElement("radius");
    centerElement->SetAttribute("r", m_R);
    element->InsertEndChild(centerElement);
}

void SerializableSphereGraphicObject::Deserialize(tinyxml2::XMLElement* element)
{

    const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
    if (attr)
        while (attr)
        {
            std::string attrValue = std::string(attr->Value());
            m_MaterialNames.push_back(attrValue);
            attr = attr->Next();
        }

    tinyxml2::XMLElement* paramElement = element->FirstChildElement();
    m_Center = glm::vec3(ParsePos(std::string(paramElement->Attribute("p"))));//paramElement->FloatAttribute("x"), paramElement->FloatAttribute("y"), paramElement->FloatAttribute("z"));
    paramElement = paramElement->NextSiblingElement();
    m_R = paramElement->FloatAttribute("r");

    m_MatX = element->FirstChildElement("matx")->UnsignedAttribute("x");
}

SphereGraphicsObjectHandler::HandleResult SphereGraphicsObjectHandler::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const
{
    HandleResult result;
    std::string nm = std::string(sceneGraphElement->Name());
    if (std::string(sceneGraphElement->Name()) != m_TypeName)
        return result;

    SerializableSphereGraphicObject * serializableSphereGraphicObject = popNew(SerializableSphereGraphicObject)(sceneGraphElement);
    result.m_SerializableGraphicObject = serializableSphereGraphicObject;

    const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });

    std::vector<std::string> attrs;
    size_t i = 0;
    const char* currentAtttrControl = 0;
    const char* currentAtttr = 0;

    while (currentAtttrControl = sceneGraphElement->Attribute((std::string("material") + std::to_string(i)).c_str(), currentAtttr))
    {
        result.m_GraphicsObjectSignature.m_Materials.push_back(materialsManager->GetMaterial(sceneGraphElement->Attribute((std::string("material") + std::to_string(i)).c_str())));
        i++;
    }

    result.m_GraphicsObjectSignature.m_SerializableGraphicsObject = serializableSphereGraphicObject;
    result.m_GraphicsObjectSignature.m_VertexFormat = vertexFormat;
    result.m_GraphicsObjectSignature.m_XMLElement = sceneGraphElement;
    result.m_GraphicsObjectSignature.m_VertexDataStream = std::make_shared<VertexDataStream>(result.m_GraphicsObjectSignature, serializableSphereGraphicObject->m_Center, serializableSphereGraphicObject->m_R, serializableSphereGraphicObject->m_MatX);
    result.m_GraphicsObjectSignature.m_Valid = true;

    return result;
}

SphereGraphicsObjectHandler::SphereGraphicsObjectHandler() : OrdinaryGraphicsObjectHandler("sphere") {}


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

SphereGraphicsObjectHandler::VertexDataStream::VertexDataStream()
{}
SphereGraphicsObjectHandler::VertexDataStream::VertexDataStream(const OrdinaryGraphicsObjectSignature& signature, glm::vec3 center, float r, glm::uint matX) : m_Signature(signature), m_Center(center), m_R(r), m_MatX(matX)
{}

void SphereGraphicsObjectHandler::VertexDataStream::Open()
{
    const size_t stepsNum = 20;
    float radL = 0.0;
    float maxRadL = glm::pi<float>();
    float radLStep = maxRadL / stepsNum;

    float l = m_Center.z - m_R;
    float maxL = 2 * m_R;
    float lStep =( maxL) / stepsNum;

    float minRadStep = (2 * glm::pi<float>()) / stepsNum;

    m_NumVertexes = 6 * (stepsNum) * (stepsNum);
    m_Vertexes.reserve(m_NumVertexes);

    int majCount = 0;
    for (float currentSlice = l; currentSlice < maxL + l; currentSlice += lStep)
    {
        float majR = m_R * (glm::sin(radL));
        float majRNext = m_R * (glm::sin(radL + radLStep));
        float nextSlice = currentSlice + lStep;
        int minCount = 0;
        for (float minRad = 0.0; minRad < 2 * glm::pi<float>(); minRad += minRadStep)
        {
            glm::vec4 spherePosOffset = glm::vec4(m_Center.x, m_Center.y, 0.0, 0.0);
            glm::vec4 v1 = glm::vec4(glm::cos(minRad) * majR, glm::sin(minRad) * majR, cos(radL)*m_R + l + m_R, 1.0) + spherePosOffset;
            glm::vec3 n1 = glm::vec3(v1) - m_Center;
            SerializableSphereGraphicObject::VertexType vertex1 = SerializableSphereGraphicObject::VertexType(v1, n1, m_MatX);

            glm::vec4 v2 = glm::vec4(glm::cos(minRad) * majRNext, glm::sin(minRad) * majRNext, cos(radL+radLStep) * m_R + l + m_R, 1.0) + spherePosOffset;
            glm::vec3 n2 = glm::vec3(v2) - m_Center;
            SerializableSphereGraphicObject::VertexType vertex2 = SerializableSphereGraphicObject::VertexType(v2, n2, m_MatX);

            glm::vec4 v3 = glm::vec4(glm::cos(minRad + minRadStep) * majRNext, glm::sin(minRad + minRadStep) * majRNext, cos(radL + radLStep) * m_R + l + m_R, 1.0) + spherePosOffset;
            glm::vec3 n3 = glm::vec3(v3) - m_Center;
            SerializableSphereGraphicObject::VertexType vertex3 = SerializableSphereGraphicObject::VertexType(v3, n3, m_MatX);

            glm::vec4 v4 = glm::vec4(glm::cos(minRad + minRadStep) * majR, glm::sin(minRad + minRadStep) * majR, cos(radL) * m_R + l + m_R, 1.0) + spherePosOffset;
            glm::vec3 n4 = glm::vec3(v4) - m_Center;
            SerializableSphereGraphicObject::VertexType vertex4 = SerializableSphereGraphicObject::VertexType(v4, n4, m_MatX);

            m_Vertexes.push_back(vertex1);
            m_Vertexes.push_back(vertex2);
            m_Vertexes.push_back(vertex3);

            m_Vertexes.push_back(vertex3);
            m_Vertexes.push_back(vertex4);
            m_Vertexes.push_back(vertex1);

            minCount += 6;
        }
        majCount++;
        radL += radLStep;
    }
}
void SphereGraphicsObjectHandler::VertexDataStream::Close()
{
}
void SphereGraphicsObjectHandler::VertexDataStream::WriteTo(VertexData& data, size_t vertexesOffset)
{
    SerializableTriangleGraphicObject* serializableGraphicsObject = (SerializableTriangleGraphicObject*)m_Signature.m_SerializableGraphicsObject;
    memcpy((void*)((SerializableTriangleGraphicObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset), m_Vertexes.data(), sizeof(SerializableTriangleGraphicObject::VertexType) * m_NumVertexes);
}

OrdinaryGraphicsObjectManager::OrdinaryGraphicsObjectManager() : m_Handlers({ popNew(TriangleGraphicsObjectHandler)(), popNew(SphereGraphicsObjectHandler)() })
{
}
OrdinaryGraphicsObjectManager::~OrdinaryGraphicsObjectManager()
{
	for (OrdinaryGraphicsObjectHandler* handler : m_Handlers)
		if (handler)
			popDelete(handler);
}

GraphicsObjectManager::HandleResult OrdinaryGraphicsObjectManager::Handle(GraphicsDevice& device, GraphicsTextureCollection& textureColection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	return HandleResult();
}


OrdinaryGraphicsObjectHandler::HandleResult OrdinaryGraphicsObjectManager::Handle_(GraphicsDevice& device, GraphicsTextureCollection& textureColection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
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

void OrdinaryGraphicsObjectManager::Render(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::vector<GraphicsShaderMacro>& passMacros, const Camera& camera)
{
	for (GraphicsObject& object : m_Objects)
	{
        if (object.m_Material != nullptr)
            object.m_Material->Bind(device, shadersCollection, passMacros);
        else
            object.m_Materials[0]->Bind(device, shadersCollection, passMacros);

		object.m_Topology.Bind(device, object.m_Materials[0]->GetBuffer(), camera, Topology_Basic);

		object.m_Topology.DrawCall(device);
	}
}