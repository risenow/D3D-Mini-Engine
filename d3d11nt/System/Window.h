#pragma once
#include <Windows.h>
#include <string>
#include <mutex>
#include "System/MemoryManager.h"
#include "System/stlcontainersintegration.h"
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

class WindowThreadCommand
{
public:
	virtual void Execute() = 0;
};

class SetCursorPosCommand : public WindowThreadCommand
{
public:
	SetCursorPosCommand(unsigned long x, unsigned long y) : m_X(x), m_Y(y)
	{
	}

	virtual void Execute() override
	{
		SetCursorPos(m_X, m_Y);
	}
private:
	unsigned long m_X;
	unsigned long m_Y;
};

class SetCursorVisibilityCommand : public WindowThreadCommand
{
public:
	SetCursorVisibilityCommand(bool visibility) : m_Visible(visibility)
	{
	}

	virtual void Execute() override
	{
		ShowCursor(m_Visible);
	}
private:
	bool m_Visible;
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

	unsigned long GetCursorX() const;
	unsigned long GetCursorY() const;

    void SwitchMode();

	template<class T>
	void AddCommand(const T& command)
	{
		m_WindowThreadCommandsMutex.lock();
		m_WindowThreadCommands.push(popNew(T)(command));
		m_WindowThreadCommandsMutex.unlock();
	}

    virtual void InitializeIniProperties();
private:
	void UpdateCursorPos();
    void WindowCreationDebugOutput();

    static void WindowThreadWorker(Window* owner);
    static void InternalCreateWindow(Window* target);
    static HINSTANCE GetConsoleHInstance();

    static void UpdateLoop(Window* target);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

	void ExecuteWindowThreadCommands();

	std::mutex m_WindowThreadCommandsMutex;
	STLStack<WindowThreadCommand*> m_WindowThreadCommands;

    size_t m_Width;
    size_t m_Height;

    unsigned int m_X;
    unsigned int m_Y;

	unsigned long m_CursorX;
	unsigned long m_CursorY;

	std::thread::id m_WindowThreadID;

    std::string m_Title;

    HWND m_WindowHandle;

    WindowMode m_WindowMode;

    bool m_Closed;
};