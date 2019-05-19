#include "stdafx.h"
#include "Camera.h"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtc/quaternion.hpp"
#include "Extern/glm/gtx/matrix_interpolation.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& rotation) : m_Position(position), m_Rotation(rotation) 
{
    m_ViewVec = glm::vec3(0.0, 0.0, -1.0);
    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
    static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
    
    m_ViewMatrix = glm::mat4x4();
    
    m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.y, yAxis);
    m_ViewMatrix = glm::translate(m_ViewMatrix, m_Position);

    //glm::vec4 pos(0.6, 0.5, -10.5, 1.0);
    //pos = pos * translatedview;

    m_ViewVec = glm::vec3(0.0, 0.0, 1.0);
    m_ViewVec = m_ViewVec * glm::mat3x3(m_ViewMatrix);
    m_LeftVec = glm::vec3(1.0, 0.0, 0.0);
    m_LeftVec = m_LeftVec*glm::mat3x3(m_ViewMatrix);
    //m_ViewMatrix = glm::inverse(m_ViewMatrix);

    m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

void Camera::StepForward(float step)
{
    m_Position += m_ViewVec * step;
}

void Camera::StepLeft(float step)
{
    m_Position += m_LeftVec * step;
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
void Camera::SetProjection(const glm::mat4x4& projectionMatrix)
{
    m_ProjectionMatrix = projectionMatrix;
    UpdateViewProjectionMatrix();
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