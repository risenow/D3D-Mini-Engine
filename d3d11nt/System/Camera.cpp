#include "stdafx.h"
#include "Camera.h"
#include "Extern/glm/gtc/matrix_transform.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& rotation) : m_Position(position), m_Rotation(rotation) 
{
    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
    static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4x4 viewMatrix = glm::rotate(glm::mat4x4(), m_Rotation.x, xAxis); //order?
    viewMatrix = glm::rotate(viewMatrix, m_Rotation.y, yAxis);
    viewMatrix = glm::rotate(viewMatrix, m_Rotation.z, zAxis);
    viewMatrix = glm::translate(viewMatrix, -m_Position); //sign?
    //m_ViewProjectionMatrix = viewMatrix * m_ProjectionMatrix;
	m_ViewProjectionMatrix = m_ProjectionMatrix * viewMatrix;
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

const glm::mat4x4& Camera::GetViewProjectionMatrix() const
{
    return m_ViewProjectionMatrix;
}