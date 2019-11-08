#pragma once
#include "System/Camera.h"
#include "System/Window.h"
#include "System/IniSerializable.h"

static const float DEFAULT_MOUSE_SENSETIVE = 0.001f;

class MouseKeyboardCameraController : public IniSerializable
{
public:
	MouseKeyboardCameraController(const Camera& camera, IniFile& ini);

	void Update(Window& window);
	Camera& GetCamera()
	{
		return m_Camera;
	}
    bool GetMouseCameraRotationActive() const
    {
        return m_MouseCameraRotationActive;
    }

	virtual void InitializeIniProperties();
private:
	float m_Sensetive;
	float m_RotationAxisY;
	float m_RotationAxisX;

    bool m_MouseCameraRotationActive;
	Camera m_Camera;
};