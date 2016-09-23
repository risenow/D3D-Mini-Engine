#include "stdafx.h"
#include "Window.h"
#include <cstring>
#include <thread>
#include "strutils.h"
#include "Log.h"
#include <limits>
#include <iostream>

WindowAttributes::WindowAttributes() : m_Width(0), m_Height(0), m_X(0), m_Y(0)
{}
WindowAttributes::WindowAttributes(const std::string& title, unsigned int posX, unsigned int posY, size_t width, size_t height) : m_Width(width), m_Height(height), m_X(posX), m_Y(posY), m_Title(title)
{}
WindowAttributes::WindowAttributes(const IniFile& info)
{
    std::string section = "Window";
    NumberRange<int> unsignedIntRange = NumberRange<int>(0, std::numeric_limits<unsigned int>::max());

    AddProperty(new IniIntProperty(section, "WindowWidth", 0, (int*)&m_Width, unsignedIntRange));
    AddProperty(new IniIntProperty(section, "WindowHeight", 0, (int*)&m_Height, unsignedIntRange));
    AddProperty(new IniIntProperty(section, "WindowPosX", 0, (int*)&m_X, unsignedIntRange));
    AddProperty(new IniIntProperty(section, "WindowPosY", 0, (int*)&m_Y, unsignedIntRange));
    AddProperty(new IniIntProperty(section, "WindowPosX", 0, (int*)&m_X, unsignedIntRange));
    AddProperty(new IniStringProperty(section, "WindowTitle", "", &m_Title));
    DeserializeFromIni(info);
}
void WindowAttributes::DeserializeFromIni(const IniFile& info)
{
    std::string section = "Window";
    int iDefault = 0;
    std::string sDefault = "";
    m_Width = info.GetUIntValue(section, "WindowWidth", iDefault);
    m_Height = info.GetUIntValue(section, "WindowHeight", iDefault);
    m_X = info.GetUIntValue(section, "WindowPosX", iDefault);
    m_Y = info.GetUIntValue(section, "WindowPosY", iDefault);
    m_Title = info.GetValue(section, "WindowTitle", sDefault);
}

Window::Window() : m_Width(DEFAULT_WINDOW_WIDTH), m_Height(DEFAULT_WINDOW_HEIGHT), 
                   m_X(DEFAULT_WINDOW_POS_X), m_Y(DEFAULT_WINDOW_POS_Y), 
                   m_Title(DEFAULT_WINDOW_TITLE), m_Closed(false), 
                   m_WindowMode(WINDOW_MODE_WINDOWED)
{
    InternalCreateWindow(this);
}

Window::Window(const std::string& title, unsigned int posX, unsigned int posY, 
               size_t width, size_t height) 
             : m_Width(width), m_Height(height), 
               m_X(posX), m_Y(posY), 
               m_Title(title), m_Closed(false), 
               m_WindowMode(WINDOW_MODE_WINDOWED)
{
    InternalCreateWindow(this);
}

Window::Window(const WindowAttributes& attributes) : m_Width(attributes.GetWidth()), m_Height(attributes.GetHeight()), 
                                                     m_X(attributes.GetPosX()), m_Y(attributes.GetPosY()), 
                                                     m_Title(attributes.GetTitle()), m_Closed(false), 
                                                     m_WindowMode(WINDOW_MODE_WINDOWED)
{
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
    windowThreadWorker.detach();
}

void Window::InternalCreateWindow(Window* target)
{
    HINSTANCE hInstance = GetConsoleHInstance();

    const static std::wstring windowClassName = L"_D3DAPP_MAIN_WINDOW";

    WNDCLASS wc = { 0 };
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = Window::WndProc;
    wc.lpszClassName = windowClassName.c_str();
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClass(&wc))
    {
        LOG(std::string("Failed to register window class"));
        exit(-1);
    }

    target->m_WindowHandle = CreateWindow(windowClassName.c_str(),
        strtowstr(target->m_Title).c_str(),
        WS_OVERLAPPEDWINDOW,
        target->m_X, target->m_Y, target->m_Width, target->m_Height,
        NULL,
        NULL,
        hInstance,
        NULL);
    if (!target->m_WindowHandle)
    {
        LOG(std::string("Failed to create window"));
        exit(-1);
    }

#ifdef _DEBUG
    target->WindowCreationDebugOutput();
#endif

    SetWindowLong(target->m_WindowHandle, GWLP_USERDATA, (LONG)target);

    ShowWindow(target->m_WindowHandle, SW_SHOW);
    UpdateWindow(target->m_WindowHandle);
}
void Window::WindowThreadWorker(Window* owner)
{
    InternalCreateWindow(owner);
    UpdateLoop(owner->m_WindowHandle);
}
void Window::Close()
{
    m_Closed = true;
}
bool Window::IsClosed() const
{
    return m_Closed;
}
HWND Window::GetWindowHandle() const
{
    return m_WindowHandle;
}
WindowMode Window::GetWindowMode() const
{
    return m_WindowMode;
}

size_t Window::GetWidth() const
{
    return m_Width;
}
size_t Window::GetHeight() const
{
    return m_Height;
}
unsigned int Window::GetX() const
{
    return m_X;
}
unsigned int Window::GetY() const
{
    return m_Y;
}

BoundRect Window::GetBoundRect() const
{
    return BoundRect(GetTopLeft(), m_Width, m_Height);
}
Point2D Window::GetTopLeft() const
{
    return Point2D(m_X, m_Y);
}

std::string Window::GetTitle() const
{
    return m_Title;
}

void Window::SwitchMode()
{
    m_WindowMode = (m_WindowMode == WINDOW_MODE_WINDOWED) ? WINDOW_MODE_FULLSCREEN : WINDOW_MODE_WINDOWED;
}

void Window::WindowCreationDebugOutput()
{
    std::cout << "Window created with next params: " << '\n';
    std::cout << "Window Title: " << m_Title << '\n';
    std::cout << "Window Pos X: " << m_X << '\n';
    std::cout << "Window Pos Y: " << m_Y << '\n';
    std::cout << "Window Width: " << m_Width << '\n';
    std::cout << "Window Height: " << m_Height << '\n' << '\n';
}
void Window::UpdateLoop(HWND hwnd)
{
    MSG msg;
    BOOL r;
    while (r = GetMessage(&msg, hwnd, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HINSTANCE Window::GetConsoleHInstance()
{
    const int titleCharBufferSize = 512;
    char consoleTitleTempCharBuffer[titleCharBufferSize];
    GetConsoleTitleA(consoleTitleTempCharBuffer, titleCharBufferSize);
    HWND hwndConsole = FindWindowA(NULL, consoleTitleTempCharBuffer);
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hwndConsole, GWL_HINSTANCE));

    return hInstance;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_SYSKEYDOWN:
        if (wparam == VK_RETURN)
        {
            if (HIWORD(lparam) & KF_ALTDOWN)
            {
                ((Window*)GetWindowLong(hwnd, GWLP_USERDATA))->SwitchMode();
            }
        }
        break;
    case WM_SIZE:
    {
        switch (wparam)
        {
        case SIZE_RESTORED:
        {
            Window* window = ((Window*)GetWindowLong(hwnd, GWLP_USERDATA));
            window->m_Width = LOWORD(lparam);
            window->m_Height = HIWORD(lparam);
            break;
        }
        }

        break;
    }

    case WM_CLOSE:
    {
        ((Window*)GetWindowLong(hwnd, GWLP_USERDATA))->Close();
        break;
    }

    case WM_PAINT:
    {
        break;
    }
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}