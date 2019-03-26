#include "stdafx.h"
#include "Graphics/GraphicsObjectHandler.h"

GraphicsObjectHandler::GraphicsObjectHandler(const std::string& typeName) : m_TypeName(typeName) {}

bool GraphicsObjectHandler::CanHandle(tinyxml2::XMLElement* sceneGraphElement) const
{
	return sceneGraphElement->Name() == m_TypeName;
}