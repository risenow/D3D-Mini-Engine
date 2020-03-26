#include "stdafx.h"
#include "Camera.h"

#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtc/quaternion.hpp"
#include "Extern/glm/gtx/matrix_interpolation.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& rotation) : m_Position(position), m_Rotation(rotation)
{
    UpdateViewProjectionMatrix();
    m_UseAngleParametrization = true;
    m_ExpEyeX = 0.0;
}

glm::vec3 GetRow3D(const glm::mat4x4& m, size_t i)
{
    return glm::vec3(m[0][i], m[1][i], m[2][i]);
}

glm::mat4x4 ViewIdentity()
{
    //invalid, directx uses left-handed coordinate system
    glm::mat4x4 m;
    m[0][0] = -1;
    m[2][2] = -1;
    return m;
}

void Camera::UpdateViewProjectionMatrix()
{
    static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    m_ViewMatrix = ViewIdentity();//glm::mat4x4();
    //m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.x, xAxis);
    //m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.y, yAxis);
    m_Eye = glm::normalize(glm::vec3(m_ExpEyeX, m_ExpEyeY, 1.0));//glm::normalize(glm::vec3(1.0, 0.7, 1.0));
    glm::mat4x4 tempView = (glm::lookAt(glm::vec3(0, 0, 0), m_Eye, glm::vec3(0.0, 1.0, 0.0)));
    if (!m_UseAngleParametrization)
    {
        m_ViewMatrix = (glm::lookAt(glm::vec3(0, 0, 0), m_Eye, glm::vec3(0.0, 1.0, 0.0)));
    }
    else
    {
        //m_ViewMatrix = (glm::transpose(m_ViewMatrix));

        //m_ViewMatrix = glm::transpose(m_ViewMatrix); //it should be here by math if we want to write mat*v

        //m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.x, xAxis);
        //m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.y, yAxis);
        glm::mat4x4 tempPrev = glm::transpose(m_ViewMatrix);
        m_ViewVec = glm::vec3(0.0, 0.0, 1.0);//tempPrev[2];

        glm::vec3 eyeXZ = m_Eye;
        eyeXZ.y = 0;

        float signXZ = -glm::sign(glm::cross(eyeXZ, m_Eye).x);//if positive rotate counter clockwise

        eyeXZ = glm::normalize(eyeXZ);

        glm::vec3 eyeZY = m_Eye;
        eyeZY.x = 0;
        float signZY = glm::sign(glm::cross(eyeXZ, glm::vec3(m_ViewVec.x, 0.0, m_ViewVec.z)).y);
        eyeZY = glm::normalize(eyeZY);

        float angleXZ = signXZ * acos(glm::dot(eyeXZ, m_Eye));// * (180.0/glm::pi<float>());
        float angleZY = signZY * acos(glm::dot(eyeXZ, glm::vec3(m_ViewVec.x, 0.0f, m_ViewVec.z)));// * (180.0/glm::pi<float>());

        m_ViewMatrix = glm::rotate(m_ViewMatrix, angleXZ, xAxis);
        m_ViewMatrix = glm::rotate(m_ViewMatrix, angleZY, yAxis);
        glm::vec3 eyeRecalc = glm::vec3(glm::vec4(0.0, 0.0, 1.0, 0.0) * m_ViewMatrix);
        //m_ViewMatrix = glm::inverse(m_ViewMatrix);
        //m_ViewMatrix = (glm::translate(m_ViewMatrix, -m_Position));
    }
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

    //invalid, directx uses left-handed coordinate system
    m_ProjectionMatrix = (glm::perspectiveFov(glm::radians(m_Fov), aspect, 1.0f, m_Near, m_Far));

    UpdateViewProjectionMatrix();
}

void Camera::SetProjectionAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
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
