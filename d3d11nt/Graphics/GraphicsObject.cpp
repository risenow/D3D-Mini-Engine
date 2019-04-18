#include "stdafx.h"
#include "Graphics/GraphicsObject.h"

GraphicsObject::GraphicsObject() : m_Valid(false) {}

bool GraphicsObject::IsValid() const 
{
	return m_Valid;
}

bool GraphicsObject::IsBatched() const { return true; }