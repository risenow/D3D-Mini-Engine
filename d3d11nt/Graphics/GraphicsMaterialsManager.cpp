#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/GraphicsMaterialsManager.h"
#include "Graphics/LightingMaterial.h"

GraphicsMaterialsManager::GraphicsMaterialsManager() : m_Handlers({ GraphicsLightingMaterial::Handle })
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
GraphicsObjectManager::HandleResult GraphicsMaterialsManager::Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)
{
	for (GraphicsMaterialHandleFunc handle : m_Handlers)
	{
		GraphicsMaterial* material = handle(device, textureCollection, shadersCollection, sceneGraphElement);
		if (material)
		{
			m_Materials[material->GetName()] = material;
			return HandleResult(true, material); // to fix nullptr
		}
	}
	return HandleResult(false, nullptr);
}
void GraphicsMaterialsManager::Add(GraphicsMaterial* material)
{
    std::string key = material->GetName();;
    if ((m_Materials.find(material->GetName()) != m_Materials.end()))
        key += '_';
    m_Materials[key] = material;
}
void GraphicsMaterialsManager::Update(const std::vector<GraphicsLightObject>& lights)
{
    for (std::pair<const std::string, GraphicsMaterial*>& m : m_Materials)
        m.second->Update(lights);
}