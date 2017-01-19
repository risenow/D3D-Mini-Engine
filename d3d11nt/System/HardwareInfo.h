#pragma once

class HardwareInfo
{
public:
    static HardwareInfo& GetInstance();

    void Initialize();

    unsigned int GetScreenWidth();
    unsigned int GetScreenHeight();
private:
    HardwareInfo();

    unsigned int m_ScreenWidth;
    unsigned int m_ScreenHeight;
};

#define popGetScreenWidth() HardwareInfo::GetInstance().GetScreenWidth()
#define popGetScreenHeight() HardwareInfo::GetInstance().GetScreenHeight()