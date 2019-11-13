#include "stdafx.h"
#include <set>
#include "GraphicsObject.h"
#include "System/MemoryManager.h"
#include "System/hashutils.h"
#include "Graphics/OrdinaryGraphicsObjectManager.h"
#include "Graphics/GraphicsMaterialsManager.h"

class GraphicObject; //immediate drawable

//create SceneDrawableObject:SerializableGraphicObject
//dynamic material indexes generation !!!! todo

void OrdinaryGraphicsObject::SetTransform(const glm::mat4x4& t) const
{
    assert(m_Drawable);
    m_Drawable->m_Transform = t;
};

// preparation for several batches
//+ realtime material index calc
struct OrdinaryObjectsKey
{
    VertexFormat m_VertexFormat;
    std::vector<GraphicsMaterial*> m_Materials; // reduce to store only materials type

    FNVhash_t Hash()
    {
        FNVhash_t r = FNV((char*)& m_VertexFormat, sizeof(VertexFormat));
        for (GraphicsMaterial* m : m_Materials)
            r ^= FNV((char*)&m, sizeof(GraphicsMaterial*));
        return r;
    }
};

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

TriangleOrdinaryGraphicsObject::TriangleOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element)
{
    Deserialize(element);

    const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });

    std::vector<std::string> attrs;
    size_t i = 0;
    const char* currentAtttrControl = 0;
    const char* currentAtttr = 0;

    while (currentAtttrControl = element->Attribute((std::string("material") + std::to_string(i)).c_str(), currentAtttr))
    {
        m_Materials.push_back(materialsManager->GetMaterial(element->Attribute((std::string("material") + std::to_string(i)).c_str())));
        i++;
    }

    m_VertexFormat = vertexFormat;
    m_XMLElement = element;

    m_VertexData = VertexData(m_VertexFormat, NumVertexes);
    memcpy((void*)((TriangleOrdinaryGraphicsObject::VertexType*)m_VertexData.GetDataPtrForSlot(0)), m_Vertexes.data(), sizeof(TriangleOrdinaryGraphicsObject::VertexType) * NumVertexes);
}
//VertexData& TriangleOrdinaryGraphicsObject::GetVertexData()
TriangleOrdinaryGraphicsObject::VertexType  TriangleOrdinaryGraphicsObject::ParseVertex(tinyxml2::XMLElement* vertexElement)
{
    return TriangleOrdinaryGraphicsObject::VertexType({ ParsePos(std::string(vertexElement->Attribute("p"))), glm::vec3(ParsePos(std::string(vertexElement->Attribute("n")))), vertexElement->UnsignedAttribute("uv", 0) });

}
void TriangleOrdinaryGraphicsObject::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
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
void TriangleOrdinaryGraphicsObject::Deserialize(tinyxml2::XMLElement* element)
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
size_t TriangleOrdinaryGraphicsObject::GetNumVertexes() const
{
    return NumVertexes;
}
void TriangleOrdinaryGraphicsObject::WriteGeometry(VertexData& data, size_t vertexesOffset)
{
    memcpy((void*)((TriangleOrdinaryGraphicsObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset), m_Vertexes.data(), sizeof(TriangleOrdinaryGraphicsObject::VertexType) * NumVertexes);
}


TriangleGraphicsObjectHandler::HandleResult TriangleGraphicsObjectHandler::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const
{
    HandleResult result = { nullptr };
	if (std::string(sceneGraphElement->Name()) != m_TypeName)
		return result;

    TriangleOrdinaryGraphicsObject* triangleOrdinaryGraphicsObject = popNew(TriangleOrdinaryGraphicsObject)(materialsManager, sceneGraphElement);
	result.m_OrdinaryGraphicsObject = triangleOrdinaryGraphicsObject;

	return result;
}

void GenerateSphereGeometry(const glm::vec3& center, float r, size_t matX, std::vector<SphereOrdinaryGraphicsObject::VertexType>& vertexes)
{
    const size_t stepsNum = 20;//30;
    float radL = 0.0;
    float maxRadL = glm::pi<float>();
    float radLStep = maxRadL / stepsNum;

    float l = center.z - r;
    float maxL = 2 * r;
    float lStep = (maxL) / stepsNum;

    float minRadStep = (2 * glm::pi<float>()) / stepsNum;

    size_t numVertexes = 6 * (stepsNum) * (stepsNum);
    vertexes.reserve(numVertexes);

    int majCount = 0;
    for (float currentSlice = l; currentSlice < maxL + l; currentSlice += lStep)
    {
        float majR = r * (glm::sin(radL));
        float majRNext = r * (glm::sin(radL + radLStep));
        float nextSlice = currentSlice + lStep;
        int minCount = 0;
        for (float minRad = 0.0; minRad < 2 * glm::pi<float>(); minRad += minRadStep)
        {
            glm::vec4 spherePosOffset = glm::vec4(center.x, center.y, 0.0, 0.0);
            glm::vec4 v1 = glm::vec4(glm::cos(minRad) * majR, glm::sin(minRad) * majR, cos(radL) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n1 = glm::vec3(v1) - center;
            SphereOrdinaryGraphicsObject::VertexType vertex1 = SphereOrdinaryGraphicsObject::VertexType(v1, n1, matX);

            glm::vec4 v2 = glm::vec4(glm::cos(minRad) * majRNext, glm::sin(minRad) * majRNext, cos(radL + radLStep) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n2 = glm::vec3(v2) - center;
            SphereOrdinaryGraphicsObject::VertexType vertex2 = SphereOrdinaryGraphicsObject::VertexType(v2, n2, matX);

            glm::vec4 v3 = glm::vec4(glm::cos(minRad + minRadStep) * majRNext, glm::sin(minRad + minRadStep) * majRNext, cos(radL + radLStep) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n3 = glm::vec3(v3) - center;
            SphereOrdinaryGraphicsObject::VertexType vertex3 = SphereOrdinaryGraphicsObject::VertexType(v3, n3, matX);

            glm::vec4 v4 = glm::vec4(glm::cos(minRad + minRadStep) * majR, glm::sin(minRad + minRadStep) * majR, cos(radL) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n4 = glm::vec3(v4) - center;
            SphereOrdinaryGraphicsObject::VertexType vertex4 = SphereOrdinaryGraphicsObject::VertexType(v4, n4, matX);

            vertexes.push_back(vertex1);
            vertexes.push_back(vertex2);
            vertexes.push_back(vertex3);

            vertexes.push_back(vertex3);
            vertexes.push_back(vertex4);
            vertexes.push_back(vertex1);

            minCount += 6;
        }
        majCount++;
        radL += radLStep;
    }
}

SphereOrdinaryGraphicsObject::SphereOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element)
{
    Deserialize(element);

    const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });

    std::vector<std::string> attrs;
    size_t i = 0;
    const char* currentAtttrControl = 0;
    const char* currentAtttr = 0;

    while (currentAtttrControl = element->Attribute((std::string("material") + std::to_string(i)).c_str(), currentAtttr))
    {
        m_Materials.push_back(materialsManager->GetMaterial(element->Attribute((std::string("material") + std::to_string(i)).c_str())));
        i++;
    }

    m_VertexFormat = vertexFormat;
    m_XMLElement = element;

    GenerateSphereGeometry(m_Center, m_R, m_MatX, m_Vertexes);
}
size_t SphereOrdinaryGraphicsObject::GetNumVertexes() const
{
    return m_Vertexes.size();
}
void SphereOrdinaryGraphicsObject::WriteGeometry(VertexData& data, size_t vertexesOffset)
{
    memcpy((void*)((SphereOrdinaryGraphicsObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset), m_Vertexes.data(), sizeof(SphereOrdinaryGraphicsObject::VertexType) * m_Vertexes.size());
}

void SphereOrdinaryGraphicsObject::Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document)
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

void SphereOrdinaryGraphicsObject::Deserialize(tinyxml2::XMLElement* element)
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
    HandleResult result = {nullptr};
    std::string nm = std::string(sceneGraphElement->Name());
    if (std::string(sceneGraphElement->Name()) != m_TypeName)
        return result;

    SphereOrdinaryGraphicsObject* sphereOrdinaryGraphicsObject = popNew(SphereOrdinaryGraphicsObject)(materialsManager, sceneGraphElement);
    result.m_OrdinaryGraphicsObject = sphereOrdinaryGraphicsObject;

    return result;
}

SphereGraphicsObjectHandler::SphereGraphicsObjectHandler() : OrdinaryGraphicsObjectHandler("sphere") {}

OrdinaryGraphicsObjectManager::OrdinaryGraphicsObjectManager() : m_Handlers({ popNew(TriangleGraphicsObjectHandler)(), popNew(SphereGraphicsObjectHandler)() })
{
}
OrdinaryGraphicsObjectManager::~OrdinaryGraphicsObjectManager()
{
    for (OrdinaryGraphicsObject*& obj : m_Objects)
        if (obj)
            popDelete(obj);
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
		if (handleResult.m_OrdinaryGraphicsObject)
		{
			m_Objects.push_back(handleResult.m_OrdinaryGraphicsObject);

			return handleResult;
		} 
	}
	return OrdinaryGraphicsObjectHandler::HandleResult();
}

std::map<unsigned int, std::shared_ptr<MaterialBatchStructuredBuffer>> MaterialBatchStructuredBuffers;

void OrdinaryGraphicsObjectManager::CompileGraphicObjects(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager)
{
    //expand model to have several batches
    std::set<size_t> allreadyBatched;

    for (size_t m = 0; m < m_Objects.size(); m++)
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
		for (size_t i = 0; i < m_Objects.size(); i++)
		{
            //check material types instead of themselves
			if (m_Objects[i]->m_VertexFormat == m_Objects[m]->m_VertexFormat && m_Objects[i]->m_Materials.size() == m_Objects[m]->m_Materials.size())
			{
				bool match = true;
                for (size_t k = 0; k < m_Objects[m]->m_Materials.size(); k++)
                {
                    if (m_Objects[m]->m_Materials[k] != m_Objects[i]->m_Materials[k])
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
            overallVertexesNum += m_Objects[i]->GetNumVertexes();
        }


		size_t vertexesWritten = 0;
		VertexData vertexData(m_Objects[m]->m_VertexFormat, overallVertexesNum);
		for (size_t i : batchSet)
		{
            OrdinaryGraphicsObject* obj = m_Objects[i];
			VertexFormat& vertexFormat = obj->m_VertexFormat;

            obj->WriteGeometry(vertexData, vertexesWritten);
            vertexesWritten += obj->GetNumVertexes();

            allreadyBatched.insert(i);
		}

			GraphicsObject object;
			object.m_Valid = true;
			object.m_Topology = GraphicsTopology(device, textureCollection, shadersCollection, vertexData, true);

			bool singleMaterial = false;
			if (m_Objects[m]->m_Materials.size() == 1)
			{
				object.m_Material = m_Objects[m]->m_Materials[0];
				singleMaterial = true;
			}

			if (!singleMaterial)
			{
				std::vector<GraphicsMaterial*> tempMaterials;
				for (unsigned int k = 0; k < m_Objects[m]->m_Materials.size(); k++)
					tempMaterials.push_back(m_Objects[m]->m_Materials[k]);

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

            m_DrawableObjects.push_back(object);

	}

	/*size_t i = 0;
	for (OrdinaryGraphicsObjectSignature& signature : m_ObjectSignatures)
	{
		
	}*/
}

void OrdinaryGraphicsObjectManager::Render(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::vector<GraphicsShaderMacro>& passMacros, const Camera& camera)
{
	for (GraphicsObject& object : m_DrawableObjects)
	{
        if (object.m_Material != nullptr)
            object.m_Material->Bind(device, shadersCollection, passMacros);
        else
            object.m_Materials[0]->Bind(device, shadersCollection, passMacros);

		object.m_Topology.Bind(device, shadersCollection, object.m_Materials[0]->GetBuffer(), camera, Topology_Basic);

		object.m_Topology.DrawCall(device);
	}
}