#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/OrdinaryGraphicsObjectManager.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/GraphicsObject.h"
#include "Graphics/GraphicsMaterialsManager.h"
#include <unordered_set>


SceneGraph::SceneGraph() : m_Managers({ m_OrdinaryGraphicsObjectManager = popNew(OrdinaryGraphicsObjectManager)(), m_MaterialsManager = popNew(GraphicsMaterialsManager)() }) {}
SceneGraph::SceneGraph(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& sceneGraphFilePath) : SceneGraph()
{
	tinyxml2::XMLDocument sceneGraphXML;
	tinyxml2::XMLError error = sceneGraphXML.LoadFile(sceneGraphFilePath.c_str());
	tinyxml2::XMLElement* element = sceneGraphXML.RootElement();

    std::unordered_set<tinyxml2::XMLElement*> handledElements;

	while (element)
	{
		if (std::string(element->Name()) == std::string("root0"))
		{
			element = element->FirstChildElement();
			continue;
		}

		for (GraphicsObjectManager* manager : m_Managers)
		{
            if (handledElements.find(element) == handledElements.cend())
            {
                {
                    GraphicsObjectManager::HandleResult handleResult = manager->Handle(device, textureCollection, shadersCollection, m_MaterialsManager, element);
                    if (handleResult.m_SerializableGraphicObject)
                    {
                        //id dont sure se should collect objects in this entity
                        m_SerializableGraphicsObjects.push_back(handleResult.m_SerializableGraphicObject);
                        handledElements.insert(element);
                    }
                }
            }

            if (handledElements.find(element) == handledElements.cend())
            {
                OrdinaryGraphicsObjectHandler::HandleResult handleResult = m_OrdinaryGraphicsObjectManager->Handle_(device, textureCollection, shadersCollection, m_MaterialsManager, element);
                if (handleResult.m_OrdinaryGraphicsObjects.size())
                    handledElements.insert(element);
                for (size_t i = 0; i < handleResult.m_OrdinaryGraphicsObjects.size(); i++)
                    m_SerializableGraphicsObjects.push_back(handleResult.m_OrdinaryGraphicsObjects[i]);
            }
		}

		element = element->NextSiblingElement();
	}

	m_OrdinaryGraphicsObjectManager->CompileGraphicObjects(device, textureCollection, shadersCollection, m_MaterialsManager);
}
SceneGraph::~SceneGraph()
{
	for (GraphicsObjectManager* manager : m_Managers)
		if (manager)
			popDelete(manager);
	for (SerializableGraphicsObject* serializableGraphicObject : m_SerializableGraphicsObjects)
		if (serializableGraphicObject)
			popDelete(serializableGraphicObject);
}
OrdinaryGraphicsObjectManager* SceneGraph::GetOrdinaryGraphicsObjectManager() const
{
	return m_OrdinaryGraphicsObjectManager;
}
GraphicsMaterialsManager* SceneGraph::GetMaterialManager()
{
    return m_MaterialsManager;
}