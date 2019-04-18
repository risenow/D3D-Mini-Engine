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
    
    glm::mat4x4 viewMatrix = glm::mat4x4();//glm::rotate(glm::mat4x4(), m_Rotation.x, xAxis); //order?
    viewMatrix = glm::rotate(viewMatrix, m_Rotation.x, xAxis);
    //viewMatrix = glm::rotate(viewMatrix, m_Rotation.y, yAxis);
    //viewMatrix = glm::rotate(viewMatrix, m_Rotation.z, zAxis);
    //viewMatrix = glm::translate(viewMatrix, -m_Position);
    //viewMatrix[3][3] = 1;
    //sign?

    glm::mat4x4 a;
    a[0] = glm::vec4(13.0, 3.0, 9.0, 1.0);
    a[1] = glm::vec4(2.0, 4.0, 1.0, 2.0);
    a[2] = glm::vec4(5.0, 8.0, 3.0, 3.0);
    a[3] = glm::vec4(1.0, 9.0, 2.0, 4.0);
    glm::mat4x4 b;
    b[0] = glm::vec4(6.0, 0.0, 7.0, 2.0);
    b[1] = glm::vec4(2.0, 3.0, 8.0, 1.0);
    b[2] = glm::vec4(3.0, 2.0, 9.0, 4.0);
    b[3] = glm::vec4(5.0, 1.0, 3.0, 10.0);

    glm::mat4x4 c = a * b;

    m_ViewProjectionMatrix = glm::inverse(viewMatrix) * m_ProjectionMatrix;
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

const glm::mat4x4& Camera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}