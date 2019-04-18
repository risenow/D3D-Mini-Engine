#pragma once

#include <string>
#include <vector>
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShadersCollection.h"
#include "Graphics/SerializableGraphicsObject.h"
#include "Graphics/GraphicsObjectManager.h"
#include "Graphics/OrdinaryGraphicsObjectManager.h"
//#include "Graphics/GraphicsObject.h"

class GraphicObject;

//full data processing aggregator
class SceneGraph
{
public:
	SceneGraph();
	SceneGraph(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, ShadersCollection& shadersCollection, const std::string& sceneGraphFilePath);
	//future non-graphical representation for compile-time operations: SceneGraph(const std::string& sceneGraphFilePath);

	~SceneGraph();

	OrdinaryGraphicsObjectManager* GetOrdinaryGraphicsObjectManager() const;

	void Serialize(const std::string& filePath);
    //void Deserialize(const std::string& filePath);
private:
	OrdinaryGraphicsObjectManager* m_OrdinaryGraphicsObjectManager;
	GraphicsMaterialsManager* m_MaterialsManager;
	std::vector<GraphicsObjectManager*> m_Managers;

	std::vector<SerializableGraphicsObject*> m_SerializableGraphicsObjects;
};