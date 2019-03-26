#pragma once

#include <vector>
#include "Extern/tinyxml2.h"
#include "SerializableGraphicsObject.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ShadersCollection.h"
#include <functional>
//#include "Graphics/GraphicsObject.h"

class GraphicsMaterialsManager;

class GraphicsObjectHandler
{
	GraphicsObjectHandler();


};

class GraphicsObjectManager
{
public:
	class HandleResult
	{
	public:
		HandleResult() : m_SerializableGraphicObject(nullptr), m_Handled(false) {}
		HandleResult(bool handled, SerializableGraphicsObject* object) : m_Handled(handled), m_SerializableGraphicObject(object) {}
		operator bool() const { return m_Handled; }
		bool m_Handled;
		SerializableGraphicsObject* m_SerializableGraphicObject;
		//GraphicsObjectSignature
	};

	GraphicsObjectManager();
	
	virtual HandleResult Handle(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement) = 0;

protected:
};

//std::function<bool(HandleResult)>(GraphicsDevice& device, ShadersCollection& shadersCollection, GraphicsMaterialsManager* materialsManager, tinyxml2::XMLElement* sceneGraphElement)