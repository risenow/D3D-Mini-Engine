#include "stdafx.h"
#include "GraphicsOptions.h"

GraphicsOptions::GraphicsOptions() {}
GraphicsOptions::GraphicsOptions(const IniFile& ini)
{
    AddProperty((IniProperty*)new IniEnumProperty(GRAPHICS_SECTION, "MultisampleType", 1, (int*)&m_MultisampleType, AcceptabeMultisampleTypeValues));
    DeserializeFromIni(ini);
}
GraphicsOptions::~GraphicsOptions()
{}

MultisampleType GraphicsOptions::GetMultisampleType()
{
    return m_MultisampleType;
}
void GraphicsOptions::DeserializeFromIni(const IniFile& ini)
{
    BasicDeserializeFrom(ini);
}
void GraphicsOptions::SerializeToIni(IniFile& ini)
{
    BasicSerializeTo(ini);
}