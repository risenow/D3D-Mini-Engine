#pragma once
#include <Windows.h>
#include <string>
#include "areahelper.h"
#include "IniFile.h"
#include "IniSerializable.h"

constexpr size_t DEFAULT_WINDOW_WIDTH = 512;
constexpr size_t DEFAULT_WINDOW_HEIGHT = 512;

constexpr unsigned int DEFAULT_WINDOW_POS_X = 0;
constexpr unsigned int DEFAULT_WINDOW_POS_Y = 0;

const std::string DEFAULT_WINDOW_TITLE = std::string("D3D11 Window");

enum WindowMode
{
    WINDOW_MODE_FULLSCREEN,
    WINDOW_MODE_WINDOWED,
    WINDOW_MODE_BORDERLESS
};

class WindowAttributes : public IniSerializable
{
public:
    WindowAttributes();
    WindowAttributes(const std::string& title, unsigned int posX, unsigned int posY, size_t width, size_t height);
    WindowAttributes(const IniFile& info);

    size_t GetWidth()      const { return m_Width; }
    size_t GetHeight()     const { return m_Height; }
    unsigned int GetPosX() const { return m_X; }
    unsigned int GetPosY() const { return m_Y; }
    std::string GetTitle() const { return m_Title; }

    virtual void SerializeToIni(IniFile& iniInfo) {};
    virtual void DeserializeFromIni(const IniFile& info);
private:
    size_t m_Width;
    size_t m_Height;

    unsigned int m_X;
    unsigned int m_Y;

    std::string m_Title;
};

//class for creation window in console app
class Window
{
public:
    Window();
    Window(const std::string& title, unsigned int posX, unsigned int posY, size_t width, size_t height);
    Window(const WindowAttributes& attributes);

    void Close();

    bool IsClosed() const;
    HWND GetWindowHandle() const;
    WindowMode GetWindowMode() const;

    size_t GetWidth() const;
    size_t GetHeight() const;
    unsigned int GetX() const;
    unsigned int GetY() const;

    BoundRect GetBoundRect() const;
    Point2D GetTopLeft() const;

    std::string GetTitle() const;

    void SwitchMode();
private:
    void WindowCreationDebugOutput();

    static void WindowThreadWorker(Window* owner);
    static void InternalCreateWindow(Window* target);
    static HINSTANCE GetConsoleHInstance();

    static void UpdateLoop(HWND hwnd);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    size_t m_Width;
    size_t m_Height;

    unsigned int m_X;
    unsigned int m_Y;

    std::string m_Title;

    HWND m_WindowHandle;

    WindowMode m_WindowMode;

    bool m_Closed;
};