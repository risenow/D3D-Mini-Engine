#pragma once
#include "IniSerializable.h"
#include "IniFile.h"
#include <vector>

constexpr unsigned int AcceptabeMultisampleTypeValuesCount = 4;
const std::vector<int> AcceptabeMultisampleTypeValues = {1, 2, 4, 8};

enum MultisampleType
{
    MULTISAMPLE_TYPE_NONE = 1,
    MULTISAMPLE_TYPE_2X = 2,
    MULTISAMPLE_TYPE_4X = 4,
    MULTISAMPLE_TYPE_8X = 8,
    MULTISAMPLE_TYPE_16X = 16,
    MULTISAMPLE_TYPE_32X = 32
};

class GraphicsOptions : public IniSerializable
{
public:
    GraphicsOptions();
    GraphicsOptions(const IniFile& initializeInfo);
    ~GraphicsOptions();

    virtual void DeserializeFromIni(const IniFile& ini);
    virtual void SerializeToIni(IniFile& ini);

    MultisampleType GetMultisampleType();
private:
    MultisampleType m_MultisampleType;

    const std::string GRAPHICS_SECTION = "Graphics";
};