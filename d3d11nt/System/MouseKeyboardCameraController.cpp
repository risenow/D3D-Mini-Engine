#include "stdafx.h"
#include <algorithm>
#include "Window.h"
#include "MouseKeyboardCameraController.h"

MouseKeyboardCameraController::MouseKeyboardCameraController(const Camera& camera, IniFile& ini) : m_Camera(camera), m_RotationAxisY(0.0f), m_Sensetive(DEFAULT_MOUSE_SENSETIVE) 
{
	DeserializeFromIni(ini);
}

void MouseKeyboardCameraController::Update(Window& window)
{
    const float VELOCITY = 1.0f;//0.005f;

    m_MouseCameraRotationActive = !!(GetAsyncKeyState(VK_RBUTTON));

    if (m_MouseCameraRotationActive)
    {
        unsigned long midCursorX = window.GetX() + window.GetWidth() / 2;
        unsigned long midCursorY = window.GetY() + window.GetHeight() / 2;

        long dX = window.GetCursorX() - midCursorX;
        long dY = window.GetCursorY() - midCursorY;

        window.AddCommand(SetCursorPosCommand(midCursorX, midCursorY));

        m_RotationAxisY += dX * m_Sensetive;
        m_RotationAxisX += dY * m_Sensetive;

        const float maxXAxisRotateAngle = glm::radians(75.0f);
        m_RotationAxisX = std::max(std::min(m_RotationAxisX, maxXAxisRotateAngle), -maxXAxisRotateAngle);

        m_Camera.SetRotation(glm::vec3(-m_RotationAxisX, m_RotationAxisY, 0));
    }

    if (GetAsyncKeyState(VK_UP))
    {
        m_Camera.StepForward(VELOCITY);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        m_Camera.StepForward(-VELOCITY);
    }
    if (GetAsyncKeyState(VK_LEFT))
    {
        m_Camera.StepLeft(VELOCITY);
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        m_Camera.StepLeft(-VELOCITY);
    }

    m_Camera.UpdateProjection((float)window.GetWidth() / (float)window.GetHeight());
}

void MouseKeyboardCameraController::InitializeIniProperties()
{
	const std::string section = "Controls";
	AddProperty(popNew(IniNumberProperty<float>)(section, "MouseSensetive", DEFAULT_MOUSE_SENSETIVE, &m_Sensetive));
}