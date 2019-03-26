#include "stdafx.h"
#include "System/Window.h"
#include <cstring>
#include <thread>
#include "strutils.h"
#include "System/MemoryManager.h"
#include "System/HardwareInfo.h"
#include "System/Log.h"
#include <limits>
#include <iostream>

Window::Window() : m_Width(DEFAULT_WINDOW_WIDTH), m_Height(DEFAULT_WINDOW_HEIGHT), 
                   m_X(DEFAULT_WINDOW_POS_X), m_Y(DEFAULT_WINDOW_POS_Y), 
                   m_Title(DEFAULT_WINDOW_TITLE), m_Closed(false), 
                   m_WindowMode(WindowMode_WINDOWED)
{
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
	m_WindowThreadID = windowThreadWorker.get_id();
    windowThreadWorker.detach();
}

Window::Window(const std::string& title, unsigned int posX, unsigned int posY, 
               size_t width, size_t height) 
             : m_Width(width), m_Height(height), 
               m_X(posX), m_Y(posY), 
               m_Title(title), m_Closed(false), 
               m_WindowMode(WindowMode_WINDOWED)
{
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
	m_WindowThreadID = windowThreadWorker.get_id();
    windowThreadWorker.detach();
}


Window::Window(const IniFile& ini) : m_Closed(false)
{
    DeserializeFromIni(ini);
	//Window::WindowThreadWorker(this);
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
	m_WindowThreadID = windowThreadWorker.get_id();
    windowThreadWorker.detach();
}
/*Window::Window(const WindowAttributes& attributes) : m_Width(attributes.GetWidth()), m_Height(attributes.GetHeight()), 
                                                     m_X(attributes.GetPosX()), m_Y(attributes.GetPosY()), 
                                                     m_Title(attributes.GetTitle()), m_Closed(false), 
                                                     m_WindowMode(WINDOW_MODE_WINDOWED)
{
    std::thread windowThreadWorker(&Window::WindowThreadWorker, this);
    windowThreadWorker.detach();
}*/

void Window::InternalCreateWindow(Window* target)
{
	HINSTANCE hInstance = NULL;//GetConsoleHInstance(); //idk why i set console instance earlier mb there was a reason so this comment may stand here for a while

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
    UpdateLoop(owner);
}
void Window::SetTitle(const std::string& title)
{
    m_Title = title;
    SetWindowText(m_WindowHandle, strtowstr(m_Title).c_str());
}
void Window::Close()
{
    m_Closed = true;
}
bool Window::IsClosed() const
{
    return m_Closed;
}
bool Window::IsFocused() const
{
    return (GetForegroundWindow() == m_WindowHandle);
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

unsigned long Window::GetCursorX() const
{
	return m_CursorX;
}
unsigned long Window::GetCursorY() const
{
	return m_CursorY;
}

void Window::SwitchMode()
{
    m_WindowMode = (m_WindowMode == WindowMode_WINDOWED) ? WindowMode_FULLSCREEN : WindowMode_WINDOWED;
}

void Window::InitializeIniProperties()
{
    std::string section = "Window";
    STLVector<int> windowModeAcceptableIntValues = NumberRange<int>(0, WINDOW_MODES_COUNT - 1).GetAllNumbers();

    AddProperty(popNew(IniNumberProperty<unsigned int>)(section, "WindowWidth", popGetScreenWidth(), &m_Width));
    AddProperty(popNew(IniNumberProperty<unsigned int>)(section, "WindowHeight", popGetScreenHeight(), &m_Height));
    AddProperty(popNew(IniNumberProperty<unsigned int>)(section, "WindowPosX", 0, &m_X));
    AddProperty(popNew(IniNumberProperty<unsigned int>)(section, "WindowPosY", 0, &m_Y));
    AddProperty(popNew(IniEnumProperty)(section, "WindowMode", WindowMode_WINDOWED, (int*)&m_WindowMode, windowModeAcceptableIntValues));

    AddProperty(popNew(IniStringProperty)(section, "WindowTitle", "", &m_Title));
}

void Window::WindowCreationDebugOutput()
{
    std::wstring logBuffer;
    LOG_BUFFER(std::string("Window created with next params: "),               logBuffer);
    LOG_BUFFER(std::string("Window Title: ") + m_Title,                        logBuffer);
    LOG_BUFFER(std::string("Window Pos X: ") + std::to_string(m_X),            logBuffer);
    LOG_BUFFER(std::string("Window Pos Y: ") + std::to_string(m_Y),            logBuffer);
    LOG_BUFFER(std::string("Window Width: ") + std::to_string(m_Width),        logBuffer);
    LOG_BUFFER(std::string("Window Height: ") + std::to_string(m_Height),      logBuffer);
	
	StringRuntimeLogStream logBufferStream(&logBuffer);
	popGetLogger().WriteUsingMode({ &logBufferStream }, logBuffer, RuntimeLogMode_None);
}
void Window::UpdateCursorPos()
{
	popAssert(std::this_thread::get_id() == m_WindowThreadID);

	POINT p;
	GetCursorPos(&p);
	m_CursorX = p.x;
	m_CursorY = p.y;
}
void Window::UpdateLoop(Window* target)
{
	HWND windowHandle = target->GetWindowHandle();
    MSG msg;
	BOOL r;
	while (!target->IsClosed())
	{
		target->UpdateCursorPos();
		target->ExecuteWindowThreadCommands();
		while (r = PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			((Window*)GetWindowLong(hwnd, GWLP_USERDATA))->Close();
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
	case WM_MOVE:
	{
		switch (wparam)
		{
		case SIZE_RESTORED:
		{
			Window* window = ((Window*)GetWindowLong(hwnd, GWLP_USERDATA));
			window->m_X = LOWORD(lparam);
			window->m_Y = HIWORD(lparam);
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

void Window::ExecuteWindowThreadCommands()
{
	m_WindowThreadCommandsMutex.lock();
	while (m_WindowThreadCommands.size())
	{
		m_WindowThreadCommands.top()->Execute();
		popDelete(m_WindowThreadCommands.top());
		m_WindowThreadCommands.pop();
	}
	m_WindowThreadCommandsMutex.unlock();
}