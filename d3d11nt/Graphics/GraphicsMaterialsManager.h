#pragma once
#include <map>
#include <vector>
#include "Graphics/GraphicsObjectManager.h"
#include "Graphics/GraphicsMaterial.h"
#include "Graphics/GraphicsTextureCollection.h"

class GraphicsMaterialsManager : public GraphicsObjectManager
{
public:
	GraphicsMaterialsManager();
	~GraphicsMaterialsManager();

	GraphicsMaterial* GetMaterial(const std::string& name) const;
	virtual HandleResult Handle(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement);
private:
	std::map<std::string, GraphicsMaterial*> m_Materials;
	std::vector<GraphicsMaterialHandleFunc> m_Handlers;
};