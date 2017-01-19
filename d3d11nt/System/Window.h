#pragma once
#include <Windows.h>
#include <string>
#include "System/areahelper.h"
#include "System/IniFile.h"
#include "System/IniSerializable.h"

constexpr size_t DEFAULT_WINDOW_WIDTH = 512;
constexpr size_t DEFAULT_WINDOW_HEIGHT = 512;

constexpr unsigned int DEFAULT_WINDOW_POS_X = 0;
constexpr unsigned int DEFAULT_WINDOW_POS_Y = 0;

const std::string DEFAULT_WINDOW_TITLE = std::string("D3D11 Window");

enum WindowMode
{
    WindowMode_FULLSCREEN,
    WindowMode_WINDOWED,
    WindowMode_BORDERLESS,
    WINDOW_MODES_COUNT
};


//class for creation window in console app
class Window : public IniSerializable
{
public:
    Window();
    Window(const std::string& title, unsigned int posX, unsigned int posY, size_t width, size_t height);
    //Window(const WindowAttributes& attributes);
    Window(const IniFile& ini);

    void SetTitle(const std::string& title);
    void Close();

    bool IsClosed() const;
    bool IsFocused() const;
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

    virtual void InitializeIniProperties();
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