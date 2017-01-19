#include "stdafx.h"
#include "System/HardwareInfo.h"
#include <Windows.h>

HardwareInfo& HardwareInfo::GetInstance()
{
    static HardwareInfo instance;
    return instance;
}

HardwareInfo::HardwareInfo() : m_ScreenWidth(0), m_ScreenHeight(0)
{}

void HardwareInfo::Initialize()
{
    m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
}

unsigned int HardwareInfo::GetScreenWidth()
{
    return m_ScreenWidth;
}
unsigned int HardwareInfo::GetScreenHeight()
{
    return m_ScreenHeight;
}