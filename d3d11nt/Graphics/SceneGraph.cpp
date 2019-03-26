#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/OrdinaryGraphicsObjectManager.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/GraphicsObject.h"
#include "Graphics/GraphicsMaterialsManager.h"



SceneGraph::SceneGraph() : m_Managers({ m_OrdinaryGraphicsObjectManager = popNew(OrdinaryGraphicsObjectManager)(), m_MaterialsManager = popNew(GraphicsMaterialsManager)() }) {}
SceneGraph::SceneGraph(GraphicsDevice& device, ShadersCollection& shadersCollection, const std::string& sceneGraphFilePath) : SceneGraph()
{
	tinyxml2::XMLDocument sceneGraphXML;
	tinyxml2::XMLError error = sceneGraphXML.LoadFile(sceneGraphFilePath.c_str());
	tinyxml2::XMLElement* element = sceneGraphXML.RootElement();

	while (element)
	{
		if (std::string(element->Name()) == std::string("root0"))
		{
			element = element->FirstChildElement();
			continue;
		}

		for (GraphicsObjectManager* manager : m_Managers)
		{
			if (GraphicsObjectManager::HandleResult handleResult = manager->Handle(device, shadersCollection, m_MaterialsManager, element))
			{
				if (handleResult.m_SerializableGraphicObject)
					m_SerializableGraphicsObjects.push_back(handleResult.m_SerializableGraphicObject);
			}

			//if (element->Name() == "triangle")
			//	bool f = true;
			OrdinaryGraphicsObjectHandler::HandleResult handleResult = m_OrdinaryGraphicsObjectManager->Handle_(device, shadersCollection, m_MaterialsManager, element);
				m_SerializableGraphicsObjects.push_back(handleResult.m_SerializableGraphicObject);

		}
		element = element->NextSiblingElement();
	}

	m_OrdinaryGraphicsObjectManager->CompileGraphicObjects(device, shadersCollection, m_MaterialsManager);
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