#pragma once
#include <string>
#include "Extern/tinyxml2.h"
class SerializableGraphicsObject
{
public:
	SerializableGraphicsObject() {}

	virtual void Serialize(tinyxml2::XMLElement* element, tinyxml2::XMLDocument& document) = 0;
	virtual void Deserialize(tinyxml2::XMLElement* element) = 0;

	std::string GetType() const
	{
		return m_Type;
	}
protected:
	std::string m_Type;
	std::string m_Name;
};