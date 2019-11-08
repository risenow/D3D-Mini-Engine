#include "stdafx.h"
#include "Camera.h"

#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtc/quaternion.hpp"
#include "Extern/glm/gtx/matrix_interpolation.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& rotation) : m_Position(position), m_Rotation(rotation)
{
    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
    static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
    m_ViewMatrix = glm::mat4x4();
    m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.x, xAxis);
    m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.y, yAxis);
    m_ViewMatrix = (glm::translate(m_ViewMatrix, -m_Position));

    glm::mat4x4 temp = glm::transpose(m_ViewMatrix);
    m_LeftVec = temp[0];
    m_ViewVec = temp[2];

    glm::vec2 projFactors = GetProjectionFactors();

    m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Camera::StepForward(float step)
{
    m_Position -= m_ViewVec * step;
}

void Camera::StepLeft(float step)
{
    m_Position -= m_LeftVec * step;
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    UpdateViewProjectionMatrix();
}
void Camera::SetRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;
    UpdateViewProjectionMatrix();
}

void Camera::SetPositionRotation(const glm::vec3& position, const glm::vec3& rotation)
{
    m_Position = position;
    m_Rotation = rotation;
    UpdateViewProjectionMatrix();
}

void Camera::SetProjection(float fov, float aspect, float _near, float _far) //  aspect == width/height
{
    m_Fov = fov;
    m_Aspect = aspect;
    m_Near = _near;
    m_Far = _far;

    m_ProjectionMatrix = (glm::perspectiveFov(glm::radians(m_Fov), aspect, 1.0f, m_Near, m_Far));
    UpdateViewProjectionMatrix();
}

void Camera::UpdateProjection(float aspect)
{
    m_Aspect = aspect;
    m_ProjectionMatrix = (glm::perspectiveFov(glm::radians(m_Fov), m_Aspect, 1.0f, m_Near, m_Far));
    UpdateViewProjectionMatrix();
}

const glm::vec3& Camera::GetRotation() const
{
    return m_Rotation;
}

const glm::vec3& Camera::GetPosition() const
{
    return m_Position;
}

const glm::mat4x4& Camera::GetViewMatrix() const
{
    return m_ViewMatrix;
}

const glm::mat4x4& Camera::GetViewProjectionMatrix() const
{
    return m_ViewProjectionMatrix;
}

const glm::mat4x4& Camera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

glm::vec2 Camera::GetProjectionFactors() const
{
    return glm::vec2(m_Aspect * glm::tan(glm::radians(m_Fov / 2.0f)), glm::tan(glm::radians(m_Fov / 2.0f)));
}
