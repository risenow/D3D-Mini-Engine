#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/GraphicsMaterialsManager.h"
#include "Graphics/PlainColorMaterial.h"

GraphicsMaterialsManager::GraphicsMaterialsManager() : m_Handlers({ GraphicsPlainColorMaterial::Handle })
{ }
GraphicsMaterialsManager::~GraphicsMaterialsManager() 
{
	for (decltype(m_Materials)::const_iterator materialIt = m_Materials.begin(); materialIt != m_Materials.end(); materialIt++)
		if ((*materialIt).second)
			popDelete((*materialIt).second);
}
GraphicsMaterial* GraphicsMaterialsManager::GetMaterial(const std::string& name) const
{
	decltype(m_Materials)::const_iterator materialIt = m_Materials.find(name);
	if (materialIt != m_Materials.end())
		return (*materialIt).second;
	return nullptr;
}
GraphicsObjectManager::HandleResult GraphicsMaterialsManager::Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	for (GraphicsMaterialHandleFunc handle : m_Handlers)
	{
		GraphicsMaterial* material = handle(device, shadersCollection, sceneGraphElement);
		if (material)
		{
			m_Materials[material->GetName()] = material;
			return HandleResult(true, material); // to fix nullptr
		}
	}
	return HandleResult(false, nullptr);
}