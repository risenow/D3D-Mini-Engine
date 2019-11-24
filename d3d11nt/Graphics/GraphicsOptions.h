#pragma once
#include "System/IniSerializable.h"
#include "System/IniFile.h"
#include "System/stlcontainersintegration.h"
#include <vector>

constexpr unsigned int AcceptabeMultisampleTypeValuesCount = 4;
const STLVector<int> AcceptabeMultisampleTypeValues = {1, 2, 4, 8};

enum MultisampleType
{
    MULTISAMPLE_TYPE_INVALID = 0,
    MULTISAMPLE_TYPE_NONE = 1,
    MULTISAMPLE_TYPE_2X = 2,
    MULTISAMPLE_TYPE_4X = 4,
    MULTISAMPLE_TYPE_8X = 8
};

class GraphicsOptions : public IniSerializable
{
public:
    GraphicsOptions();
    GraphicsOptions(const IniFile& initializeInfo);
    ~GraphicsOptions();

    virtual void InitializeIniProperties();

    MultisampleType GetMultisampleType();
    void SetMultisampleType(MultisampleType msType)
    {
        m_MultisampleType = msType;
    }
private:
    MultisampleType m_MultisampleType;

    const std::string GRAPHICS_SECTION = "Graphics";
};