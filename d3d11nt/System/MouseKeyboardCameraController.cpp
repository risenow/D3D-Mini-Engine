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
	unsigned long midCursorX = window.GetWidth() / 2; 
    unsigned long midCursorY = window.GetHeight() / 2;

	long dX = window.GetCursorX() - midCursorX;
	long dY = window.GetCursorY() - midCursorY;

	window.AddCommand(SetCursorPosCommand(midCursorX, midCursorY));

	m_RotationAxisY += dX * m_Sensetive;
	m_RotationAxisX += dY * m_Sensetive;

	const float maxXAxisRotateAnge = glm::radians(90.0f);
	m_RotationAxisX = std::max(std::min(m_RotationAxisX, maxXAxisRotateAnge), -maxXAxisRotateAnge);

	m_Camera.SetRotation(glm::vec3(m_RotationAxisX, m_RotationAxisY, 0));
}

void MouseKeyboardCameraController::InitializeIniProperties()
{
	const std::string section = "Controls";
	AddProperty(popNew(IniNumberProperty<float>)(section, "MouseSensetive", DEFAULT_MOUSE_SENSETIVE, &m_Sensetive));
}