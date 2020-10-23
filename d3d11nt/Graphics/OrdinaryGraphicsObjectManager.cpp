#include "stdafx.h"
#include <set>
#include "GraphicsObject.h"
#include "System/MemoryManager.h"
#include "System/hashutils.h"
#include "System/pathutils.h"
#include "Graphics/OrdinaryGraphicsObjectManager.h"
#include "Graphics/LightingGraphicsTopology.h"
#include "Graphics/GraphicsMaterialsManager.h"
#include "Extern/glm/gtc/constants.hpp"

bool operator<(const tinyobj::index_t& v1, const tinyobj::index_t& v2)
{
    return (v1.normal_index < v2.normal_index) && (v1.texcoord_index < v2.texcoord_index) && (v1.vertex_index < v2.vertex_index);
}

class GraphicObject; //immediate drawable

struct OrdinaryObjectsKey
{
    VertexFormat m_VertexFormat;
    std::vector<GraphicsMaterial*> m_Materials; // reduce to store only materials type
    bool m_IsBatchable;

    FNVhash_t Hash()
    {
        size_t slotsNum = m_VertexFormat.GetNumSlotsUsed();
        FNVhash_t r = FNV((char*)&slotsNum, sizeof(size_t));
        r ^= FNV((char*)(m_VertexFormat.m_PerSlotVertexSizes.data()), m_VertexFormat.m_PerSlotVertexSizes.size() * sizeof(size_t));

        for (GraphicsMaterial* m : m_Materials)
            r ^= FNV((char*)&m, sizeof(GraphicsMaterial*));
        return r;
    }
};
 
void OrdinaryGraphicsObject::SetTransform(const glm::mat4x4& t) const
{
    assert(m_Drawable);
    m_Drawable->m_Transform = t;
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

TriangleOrdinaryGraphicsObject::TriangleOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element) : OrdinaryGraphicsObject()
{
    Deserialize(element);

    const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });
    VertexFormat batchedVertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });

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
    m_BatchedVertexFormat = batchedVertexFormat;
    m_XMLElement = element;
}
//VertexData& TriangleOrdinaryGraphicsObject::GetVertexData()
TriangleOrdinaryGraphicsObject::VertexType  TriangleOrdinaryGraphicsObject::ParseVertex(tinyxml2::XMLElement* vertexElement)
{
    return TriangleOrdinaryGraphicsObject::VertexType({ ParsePos(std::string(vertexElement->Attribute("p"))), glm::vec3(ParsePos(std::string(vertexElement->Attribute("n")))) });

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
    //todo move to OrinaryGraphicsObject code
    const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
    if (attr)
        while (attr)
        {
            std::string attrValue = std::string(attr->Value());

            if (strstr(attr->Name(), "material"))
                m_MaterialNames.push_back(attrValue);
            if (strstr(attr->Name(), "batchable"))
                m_Batchable = attrValue == "true";

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

// to support not batched objects
void TriangleOrdinaryGraphicsObject::WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched, size_t vertexesOffset)
{
    std::vector<size_t> matRemap;
    for (GraphicsMaterial*& mat : m_Materials)
    {
        matRemap.push_back(std::distance(materialIndexRemap.begin(), materialIndexRemap.find(mat)));
    }
    if (batched)
    {
        TriangleOrdinaryGraphicsObject::BatchedVertexType* dst = (TriangleOrdinaryGraphicsObject::BatchedVertexType*)data.GetDataPtrForSlot(0) + vertexesOffset;
        for (size_t i = 0; i < m_Vertexes.size(); i++)
        {
            TriangleOrdinaryGraphicsObject::BatchedVertexType& v = (*(dst + i));
            v = BatchedVertexType(m_Vertexes[i], matRemap[0]); // suppose we have one material per object
        }
        return;
    }

    TriangleOrdinaryGraphicsObject::VertexType* dst = (TriangleOrdinaryGraphicsObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset;
    for (size_t i = 0; i < m_Vertexes.size(); i++)
    {
        TriangleOrdinaryGraphicsObject::VertexType& v = (*(dst + i));
        v = m_Vertexes[i];
    }
}


TriangleGraphicsObjectHandler::HandleResult TriangleGraphicsObjectHandler::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const
{
    HandleResult result;
    result.m_OrdinaryGraphicsObjects = { };
	if (std::string(sceneGraphElement->Name()) != m_TypeName)
		return result;

    TriangleOrdinaryGraphicsObject* triangleOrdinaryGraphicsObject = popNew(TriangleOrdinaryGraphicsObject)(materialsManager, sceneGraphElement);
	result.m_OrdinaryGraphicsObjects.push_back(triangleOrdinaryGraphicsObject);

	return result;
}

void GenerateSphereGeometry(const glm::vec3& center, float r, size_t matX, std::vector<SphereOrdinaryGraphicsObject::VertexType>& vertexes, std::vector<uint32_t>& indexes)
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
    indexes.reserve(numVertexes);

    glm::vec4 spherePosOffset = glm::vec4(center.x, center.y, 0.0, 0.0);

    int majCount = 0;
    for (float currentSlice = l; currentSlice < maxL + l; currentSlice += lStep)
    {
        float majR = r * (glm::sin(radL));
        float majRNext = r * (glm::sin(radL + radLStep));
        float nextSlice = currentSlice + lStep;

        for (float minRad = 0.0; minRad < 2 * glm::pi<float>(); minRad += minRadStep)
        {            
            glm::vec4 v1 = glm::vec4(glm::cos(minRad) * majR, glm::sin(minRad) * majR, cos(radL) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n1 = glm::normalize(glm::vec3(v1) - center);
            SphereOrdinaryGraphicsObject::VertexType vertex1 = SphereOrdinaryGraphicsObject::VertexType(v1, n1, matX);

            glm::vec4 v2 = glm::vec4(glm::cos(minRad) * majRNext, glm::sin(minRad) * majRNext, cos(radL + radLStep) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n2 = glm::normalize(glm::vec3(v2) - center);
            SphereOrdinaryGraphicsObject::VertexType vertex2 = SphereOrdinaryGraphicsObject::VertexType(v2, n2, matX);

            glm::vec4 v3 = glm::vec4(glm::cos(minRad + minRadStep) * majRNext, glm::sin(minRad + minRadStep) * majRNext, cos(radL + radLStep) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n3 = glm::normalize(glm::vec3(v3) - center);
            SphereOrdinaryGraphicsObject::VertexType vertex3 = SphereOrdinaryGraphicsObject::VertexType(v3, n3, matX);

            glm::vec4 v4 = glm::vec4(glm::cos(minRad + minRadStep) * majR, glm::sin(minRad + minRadStep) * majR, cos(radL) * r + l + r, 1.0) + spherePosOffset;
            glm::vec3 n4 = glm::normalize(glm::vec3(v4) - center);
            SphereOrdinaryGraphicsObject::VertexType vertex4 = SphereOrdinaryGraphicsObject::VertexType(v4, n4, matX);


            uint32_t v1index = vertexes.size();
            vertexes.push_back(vertex1);
            vertexes.push_back(vertex2);
            vertexes.push_back(vertex3);
            vertexes.push_back(vertex4);


            indexes.push_back(v1index);
            indexes.push_back(v1index + 1);
            indexes.push_back(v1index + 2);

            indexes.push_back(v1index + 2);
            indexes.push_back(v1index + 3);
            indexes.push_back(v1index);
        }
        majCount++;
        radL += radLStep;
    }
}

SphereOrdinaryGraphicsObject::SphereOrdinaryGraphicsObject(GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element) : OrdinaryGraphicsObject()
{
    Deserialize(element);

    const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
    const std::string POSITION_PROPERTY_NAME = "POSITION";
    typedef glm::uint LocalTexCoordType;

    VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });
    VertexFormat batchedVertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<LocalTexCoordType>(TEXCOORD_PROPERTY_NAME) });// , CreateVertexPropertyPrototype<uint32_t>(MATERIAL_PROPERTY_NAME, 0, 1) });

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
    m_BatchedVertexFormat = batchedVertexFormat;
    m_XMLElement = element;

    GenerateSphereGeometry(m_Center, m_R, m_MatX, m_Vertexes, m_Indexes);
}
size_t SphereOrdinaryGraphicsObject::GetNumVertexes() const
{
    return m_Vertexes.size();
}
void SphereOrdinaryGraphicsObject::WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched, size_t vertexesOffset)
{
    
    std::vector<size_t> matRemap;
    for (GraphicsMaterial*& mat : m_Materials)
    {
        matRemap.push_back(std::distance(materialIndexRemap.begin(), materialIndexRemap.find(mat)));
    }

    data.SetIndexes(m_Indexes);

    if (batched)
    {
        SphereOrdinaryGraphicsObject::BatchedVertexType* dst = (SphereOrdinaryGraphicsObject::BatchedVertexType*)data.GetDataPtrForSlot(0) + vertexesOffset;
        for (size_t i = 0; i < m_Vertexes.size(); i++)
        {
            SphereOrdinaryGraphicsObject::BatchedVertexType& v = (*(dst + i));
            v = BatchedVertexType(m_Vertexes[i], matRemap[0]);
        }
        return;
    }

    SphereOrdinaryGraphicsObject::VertexType* dst = (SphereOrdinaryGraphicsObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset;
    for (size_t i = 0; i < m_Vertexes.size(); i++)
    {
        SphereOrdinaryGraphicsObject::VertexType& v = (*(dst + i));
        v = m_Vertexes[i];
    }

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

            if (strstr(attr->Name(), "material"))
                m_MaterialNames.push_back(attrValue);
            if (strstr(attr->Name(), "batchable"))
                m_Batchable = attrValue == "true";

            attr = attr->Next();
        }

    tinyxml2::XMLElement* paramElement = element->FirstChildElement();
    m_Center = glm::vec3(ParsePos(std::string(paramElement->Attribute("p"))));//paramElement->FloatAttribute("x"), paramElement->FloatAttribute("y"), paramElement->FloatAttribute("z"));
    paramElement = paramElement->NextSiblingElement();
    m_R = paramElement->FloatAttribute("r");

    m_MatX = element->FirstChildElement("matx")->UnsignedAttribute("x");
}

SphereGraphicsObjectHandler::HandleResult SphereGraphicsObjectHandler::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) const
{
    HandleResult result;
    result.m_OrdinaryGraphicsObjects = { };
    std::string nm = std::string(sceneGraphElement->Name());
    if (std::string(sceneGraphElement->Name()) != m_TypeName)
        return result;

    SphereOrdinaryGraphicsObject* sphereOrdinaryGraphicsObject = popNew(SphereOrdinaryGraphicsObject)(materialsManager, sceneGraphElement);
    result.m_OrdinaryGraphicsObjects.push_back(sphereOrdinaryGraphicsObject);

    return result;
}
SphereGraphicsObjectHandler::SphereGraphicsObjectHandler() : OrdinaryGraphicsObjectHandler("sphere") {}

SubMeshModelGraphicsObject::SubMeshModelGraphicsObject(MetaModelGraphicsObject* metaObj, size_t shapeIndex) : m_MetaObj(metaObj), m_ShapeIndex(shapeIndex)
{
    const std::string BITANGENT_PROPERTY_NAME = "NORMAL";
    const std::string TANGENT_PROPERTY_NAME = "NORMAL";
    const std::string TEXCOORD_PROPERTY_NAME = "TEXCOORD";
    const std::string NORMAL_PROPERTY_NAME = "NORMAL";
    const std::string POSITION_PROPERTY_NAME = "POSITION";

    aiNode* modelNode = m_MetaObj->m_ModelScene->mRootNode;
    const aiNode* node = modelNode->mChildren[m_ShapeIndex];
    aiMesh* mesh = m_MetaObj->m_ModelScene->mMeshes[node->mMeshes[0]];

    m_Materials.push_back(m_MetaObj->m_Materials[mesh->mMaterialIndex]);

    m_XMLElement = nullptr;

    //add possibility to support arbitary format(ie no normals or no tc)
    std::vector<VertexPropertyPrototype> vertexProperties = { CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME) };
    if (mesh->HasNormals())
        vertexProperties.push_back(CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME, 0));
    if (mesh->HasTextureCoords(0))
    {
        m_TexCoordEnabled = true;
        vertexProperties.push_back(CreateVertexPropertyPrototype<glm::vec2>(TEXCOORD_PROPERTY_NAME));
    }
    if (mesh->HasTangentsAndBitangents())
    {
        m_TBNEnabled = true;
        vertexProperties.push_back(CreateVertexPropertyPrototype<glm::vec3>(TANGENT_PROPERTY_NAME, 1));
        vertexProperties.push_back(CreateVertexPropertyPrototype<glm::vec3>(BITANGENT_PROPERTY_NAME, 2));
    }

    //VertexFormat vertexFormat({ CreateVertexPropertyPrototype<glm::vec4>(POSITION_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec3>(NORMAL_PROPERTY_NAME), CreateVertexPropertyPrototype<glm::vec2>(TEXCOORD_PROPERTY_NAME) });
    VertexFormat vertexFormat(vertexProperties);

    m_VertexFormat = vertexFormat;
}
void SubMeshModelGraphicsObject::WriteGeometry(VertexData& data, const std::set<GraphicsMaterial*>& materialIndexRemap, bool batched, size_t vertexesOffset)
{
    float* dst = (float*)((SubMeshModelGraphicsObject::VertexType*)data.GetDataPtrForSlot(0) + vertexesOffset);
    aiNode* modelNode = m_MetaObj->m_ModelScene->mRootNode;
    const aiNode* node = modelNode->mChildren[m_ShapeIndex];

    if (!node->mNumMeshes)
        return;

    aiMesh* mesh = m_MetaObj->m_ModelScene->mMeshes[node->mMeshes[0]];

    bool normalsEnabled = mesh->HasNormals();
    bool tcEnabled = mesh->HasTextureCoords(0);
    bool tbnEnabled = mesh->HasTangentsAndBitangents();

    size_t stride = 4;
    if (normalsEnabled)
        stride += 3;
    if (tcEnabled)
        stride += 2;
    if (tbnEnabled)
        stride += 6;

    for (unsigned int v = 0; v < mesh->mNumVertices; v++)
    {

        aiVector3D aiPos = mesh->mVertices[v]* 0.1f;
        glm::vec4 pos = glm::vec4(aiPos.x, aiPos.y, aiPos.z, 1.0f);
        *((glm::vec4*)(dst + v * stride)) = pos;

        if (normalsEnabled)
        {
            aiVector3D aiNormal = mesh->mNormals[v];
            glm::vec3 normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
            *((glm::vec3*)(dst + v * stride + 4)) = normal;
        }

        if (tcEnabled)
        {
            aiVector3D aiTc = mesh->mTextureCoords[0][v];
            glm::vec2 tc = glm::vec2(aiTc.x, aiTc.y);
            *((glm::vec2*)(dst + v * stride + 7)) = tc;
        }
        if (tbnEnabled)
        {
            aiVector3D aiTangent = mesh->mTangents[v];
            glm::vec3 tangent = glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z);
            *((glm::vec3*)(dst + v * stride + 9)) = tangent;

            aiVector3D aiBitangent = mesh->mBitangents[v];
            glm::vec3 bitangent = glm::vec3(aiBitangent.x, aiBitangent.y, aiBitangent.z);
            *((glm::vec3*)(dst + v * stride + 11)) = bitangent;
        }
        //*(dst + v) = VertexType(pos, normal, tc);
    }

    std::vector<uint32_t> indexes;
    for (unsigned int f = 0; f < mesh->mNumFaces; f++)
    {
        aiFace* face = &mesh->mFaces[f];
        indexes.push_back(face->mIndices[0]);
        indexes.push_back(face->mIndices[1]);
        indexes.push_back(face->mIndices[2]);
    }

    std::swap(data.GetIndexes(), indexes);
}
size_t SubMeshModelGraphicsObject::GetNumVertexes() const
{
    aiNode* modelNode = m_MetaObj->m_ModelScene->mRootNode;
    const aiNode* node = modelNode->mChildren[m_ShapeIndex];

    aiMesh* mesh = m_MetaObj->m_ModelScene->mMeshes[node->mMeshes[0]];
    return mesh->mNumVertices;
}

MetaModelGraphicsObject::MetaModelGraphicsObject(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsTextureCollection& textureCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* element)
{
    Deserialize(element);
    m_ModelScene = importer.ReadFile(m_Path.c_str(), 
        //aiProcess_MakeLeftHanded | 
        aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_PreTransformVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_Triangulate |
        //aiProcess_FixInfacingNormals |
        aiProcess_FindInvalidData |
        aiProcess_ValidateDataStructure | 0

    );

    aiMaterial** materials = m_ModelScene->mMaterials;
    for (size_t i = 0; i < m_ModelScene->mNumMaterials; i++)
    {
        aiMaterial* mat = materials[i];
        aiString name;
        mat->Get(AI_MATKEY_NAME, name);
        //TBN ALWAYS TRUE?
        GraphicsMaterial* material = popNew(GraphicsLightingMaterial)(device, textureCollection, shadersCollection, std::string(name.C_Str()), LightingConstsFromAiMaterial(mat), ExtractLightingMaterialTexturesFromAiMaterial(device, textureCollection, mat, ExcludeFileFromPath(m_Path)));
        m_Materials.push_back(material);
        materialsManager->Add(material);
    }

    m_XMLElement = element;
}
void MetaModelGraphicsObject::ProduceSubMeshes(std::vector<OrdinaryGraphicsObject*>& graphicsObjects)
{
    aiNode* modelNode = m_ModelScene->mRootNode;
    for (size_t i = 0; i < modelNode->mNumChildren; i++)
    {
        graphicsObjects.push_back(popNew(SubMeshModelGraphicsObject)(this, i));
    }

}


OrdinaryGraphicsObjectManager::OrdinaryGraphicsObjectManager() : m_Handlers({ popNew(TriangleGraphicsObjectHandler)(), popNew(SphereGraphicsObjectHandler)(), popNew(ModelGraphicsObjectHandler)()})
{
}

GraphicsObjectManager::HandleResult OrdinaryGraphicsObjectManager::Handle(GraphicsDevice& device, GraphicsTextureCollection& textureColection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	return HandleResult();
}


OrdinaryGraphicsObjectHandler::HandleResult OrdinaryGraphicsObjectManager::Handle_(GraphicsDevice& device, GraphicsTextureCollection& textureColection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	for (OrdinaryGraphicsObjectHandler* handler : m_Handlers)
	{
		OrdinaryGraphicsObjectHandler::HandleResult handleResult = handler->Handle(device, shadersCollection, textureColection, materialsManager, sceneGraphElement);
        for (size_t i = 0; i < handleResult.m_OrdinaryGraphicsObjects.size(); i++)
        {
            m_Objects.push_back(handleResult.m_OrdinaryGraphicsObjects[i]);
        }

        if (handleResult.m_OrdinaryGraphicsObjects.size())
            return handleResult;
	}
	return OrdinaryGraphicsObjectHandler::HandleResult();
}


std::map<unsigned int, std::shared_ptr<MaterialBatchStructuredBuffer>> MaterialBatchStructuredBuffers;

void OrdinaryGraphicsObjectManager::CompileGraphicObjects(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager)
{
    std::set<size_t> allreadyBatched;

    for (size_t m = 0; m < m_Objects.size(); m++)
	{
        if (!m_Objects[m]->IsDrawable())
            continue;
        if (allreadyBatched.find(m) != allreadyBatched.end())
            continue;

        std::vector<uint32_t> batchSet;

        std::string materialType = m_Objects[m]->m_Materials[0]->GetType();


        std::set<GraphicsMaterial*> materialsIndexMap;
        VertexFormat basicVertexFormat = m_Objects[m]->m_VertexFormat;
        VertexFormat batchedVertexFormat = m_Objects[m]->m_BatchedVertexFormat;
        //from <m> ?
        if (m_Objects[m]->m_Batchable)
            for (size_t i = 0; i < m_Objects.size(); i++)
            {
                if (!m_Objects[i]->m_Batchable)
                    continue;
	            if (m_Objects[i]->m_BatchedVertexFormat == batchedVertexFormat)
	            {
		            bool match = true;
                    for (size_t k = 0; k < m_Objects[i]->m_Materials.size(); k++)
                    {
                        if (m_Objects[i]->m_Materials[k]->GetType() != materialType)
                            match = false;
                    }
			
                    if (match)
                    {
                        batchSet.push_back(i);
                    }
	            }
            }
        else
            batchSet.push_back(m);

     
		size_t overallVertexesNum = 0;
        for (size_t i : batchSet)
        {
            for (GraphicsMaterial* mat : m_Objects[i]->m_Materials)
                materialsIndexMap.insert(mat);
            overallVertexesNum += m_Objects[i]->GetNumVertexes();
        }

        bool batched = batchSet.size() > 1;

		size_t vertexesWritten = 0;
        VertexFormat usedVertexFormat = (batched) ? batchedVertexFormat : basicVertexFormat;
		VertexData vertexData(usedVertexFormat, overallVertexesNum);
        bool enableTBN = false;
        bool enableTC = false;
		for (size_t i : batchSet)
		{
            OrdinaryGraphicsObject* obj = m_Objects[i];

            enableTBN = enableTBN || obj->TBNEnabled();
            enableTC = enableTC || obj->TexCoordEnabled();

            obj->WriteGeometry(vertexData, materialsIndexMap, batched, vertexesWritten);
            vertexesWritten += obj->GetNumVertexes();

            allreadyBatched.insert(i);
		}


        uint32_t inputSignExtractionVariation = 0;
        if (enableTBN)
            inputSignExtractionVariation = inputSignExtractionVariation | BasicPixelShaderVariations::TBN;
        if (enableTC)
            inputSignExtractionVariation = inputSignExtractionVariation | BasicPixelShaderVariations::TEXCOORD;
		GraphicsObject object;
		object.m_Valid = true;
        object.m_Topology = popNew(TypedBasicVertexGraphicsTopology<VSConsts>)(device, shadersCollection, ShaderStrIdentifier(std::wstring(L"Test/vs.hlsl"), inputSignExtractionVariation), vertexData, Topology_Triangles, GraphicsBuffer::UsageFlag_Immutable, enableTC, enableTBN, batched);

        std::vector<GraphicsMaterial*> tempMaterials;

        for (GraphicsMaterial* mat : materialsIndexMap)
            tempMaterials.push_back(mat);

		bool singleMaterial = false;
        //todo check if not batchable
        if (tempMaterials.size() == 1 && !batched)
        {
            object.m_Material = m_Objects[m]->m_Materials[0];
            singleMaterial = true;
        }

		if (!singleMaterial && batched)
		{
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
}


void OrdinaryGraphicsObjectManager::Render(GraphicsDevice& device, ShadersCollection& shadersCollection, uint32_t passMacros, const Camera& camera, bool shadowPass)
{
	for (GraphicsObject& object : m_DrawableObjects)
	{
        GraphicsTopology* currentTopology = object.m_Topology;

        uint32_t materialPassMacros = passMacros;
        if (currentTopology->TexCoordsEnabled())
            materialPassMacros = materialPassMacros | BasicPixelShaderVariations::TEXCOORD;
        if (currentTopology->TBNEnabled())
            materialPassMacros = materialPassMacros | BasicPixelShaderVariations::TBN;

        if (!shadowPass)
        {
            if (object.m_Material != nullptr)
                object.m_Material->Bind(device, shadersCollection, camera, nullptr, materialPassMacros);
            else
                object.m_Materials[0]->Bind(device, shadersCollection, camera, nullptr, materialPassMacros);
        }
        else
            GraphicsMaterial::BindNull(device);

        VSConsts consts;
        FillLightingGraphicsTopologyConstants(consts, camera);
        object.m_Topology->Bind(device, shadersCollection, object.m_Materials.size() ? &object.m_Materials[0]->GetBuffer() : (GraphicsBuffer*)nullptr,
                                object.m_Material ? object.m_Material->GetConstantsBuffer() : nullptr, (void*)&consts);

		object.m_Topology->DrawCall(device);
	}
}

OrdinaryGraphicsObjectManager::~OrdinaryGraphicsObjectManager()
{
    for (GraphicsObject& object : m_DrawableObjects)
        if (object.m_Topology)
            popDelete(object.m_Topology);
    for (OrdinaryGraphicsObject*& obj : m_Objects)
        if (obj)
            popDelete(obj);
    for (OrdinaryGraphicsObjectHandler* handler : m_Handlers)
        if (handler)
            popDelete(handler);
}