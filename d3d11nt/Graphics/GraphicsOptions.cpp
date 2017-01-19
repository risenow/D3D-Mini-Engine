#include "stdafx.h"
#include "System/MemoryManager.h"
#include "Graphics/GraphicsOptions.h"

GraphicsOptions::GraphicsOptions() {}
GraphicsOptions::GraphicsOptions(const IniFile& ini)
{
    DeserializeFromIni(ini);
}
GraphicsOptions::~GraphicsOptions()
{}

MultisampleType GraphicsOptions::GetMultisampleType()
{
    return m_MultisampleType;
}

void GraphicsOptions::InitializeIniProperties()
{
    AddProperty((IniProperty*)popNew(IniEnumProperty)(GRAPHICS_SECTION, "MultisampleType", 1, (int*)&m_MultisampleType, AcceptabeMultisampleTypeValues));
}