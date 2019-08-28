#include "stdafx.h"
#include "Camera.h"

#include "Extern/glm/gtc/matrix_transform.hpp"
#include "Extern/glm/gtc/quaternion.hpp"
#include "Extern/glm/gtx/matrix_interpolation.hpp"
//#include <xnamath.inl>
//#include <math.h>
//#include <minwindef.h>
//#include <xnamath.h>

Camera::Camera(const SimpleMath::Vector3& position, const glm::vec3& rotation) : m_Position(position), m_Rotation(rotation)
{
    m_ViewVec = SimpleMath::Vector3(0.0, 0.0, -1.0);
    UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
    //DirectX::XMMATRIX m;
    //m = DirectX::XMMatrixRotationX()
    static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    //m_Position = SimpleMath::Vector3(0.0, 0.)
    SimpleMath::Matrix m = SimpleMath::Matrix::CreateRotationY(-m_Rotation.y);
    m_ViewMatrix = SimpleMath::Matrix::CreateRotationX(-m_Rotation.x);
    m_ViewMatrix = m_ViewMatrix * SimpleMath::Matrix::CreateRotationY(-m_Rotation.y);
    m_ViewMatrix = m_ViewMatrix * SimpleMath::Matrix::CreateTranslation(m_Position);//DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

    m_ViewVec = SimpleMath::Vector3(0.0, 0.0, 1.0);
    m_ViewVec = SimpleMath::Vector3::Transform(m_ViewVec, m_ViewMatrix);
    m_LeftVec = SimpleMath::Vector3(1.0, 0.0, 0.0);
    m_LeftVec = SimpleMath::Vector3::Transform(m_LeftVec, m_ViewMatrix);

    m_ViewMatrix =  m_ViewMatrix.Invert();//DirectX::XMMatrixInverse(nullptr, m_ViewMatrix);
    //m_ViewMatrix = m_ViewMatrix.Transpose();

    //m_ViewMatrix = glm::mat4x4();
    //m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.x, xAxis);
    //m_ViewMatrix = glm::rotate(m_ViewMatrix, m_Rotation.y, yAxis);
    //m_ViewMatrix = (glm::translate(m_ViewMatrix, -m_Position));

    //glm::mat4x4 m = glm::transpose(m_ViewMatrix);
    //m_ViewMatrix = glm::inverse(m_ViewMatrix);
    //m_ViewMatrix = glm::transpose(m_ViewMatrix);

    //m_ViewMatrix = glm::inverse(m_ViewMatrix);

    
    //m_ViewVec = SimpleMath::Vector3(0.0, 0.0, 1.0);
   // m_ViewVec = SimpleMath::Vector3::Transform(m_ViewVec, m_ViewMatrix);
   // m_LeftVec = SimpleMath::Vector3(1.0, 0.0, 0.0);
   // m_LeftVec = SimpleMath::Vector3::Transform(m_LeftVec, m_ViewMatrix);//DirectX::XMVector3Transform(m_LeftVec, m_ViewMatrix);//glm::mat3x3(m_ViewMatrix) * m_LeftVec;

    SimpleMath::Vector4 ve = SimpleMath::Vector4(0.5, 0.5, -10.5, 1.0);
    ve = SimpleMath::Vector4::Transform(ve, m_ViewMatrix);
    //vPos r0.xyzw float4 10.43221, 0.37253, -1.33428, 1.00
    glm::vec4 v = glm::vec4(1.0, 0.0, -10.0, 1.0);
    //v = m_ViewMatrix * v;
    glm::vec4 v3 = v;
    float z = v.z;

    //v = m_ProjectionMatrix * v;//m_ProjectionMatrix * v;

    //v = v / v.w;

    glm::vec2 projFactors = GetProjectionFactors();
    glm::vec4 v2;
    v2.x = v.x * ((projFactors.x * z));// * z;
    v2.y = v.y * ((projFactors.y * z));// * z;
    v2.z = z;

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


void Camera::SetPosition(const SimpleMath::Vector3& position)
{
    m_Position = position;
    UpdateViewProjectionMatrix();
}
void Camera::SetRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;
    UpdateViewProjectionMatrix();
}
void Camera::SetPositionRotation(const SimpleMath::Vector3& position, const glm::vec3& rotation)
{
    m_Position = position;
    m_Rotation = rotation;
    UpdateViewProjectionMatrix();
}
//void Camera::SetProjection(const glm::mat4x4& projectionMatrix)
//{
//    m_ProjectionMatrix = projectionMatrix;
//    UpdateViewProjectionMatrix();
//}
void Camera::SetProjection(float fov, float aspect, float _near, float _far) //  aspect == width/height
{
    m_Fov = fov;
    m_Aspect = aspect;
    m_Near = _near;
    m_Far = _far;

    m_ProjectionMatrix = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspect, _near, _far); //DirectX::XMMatrixPerspectiveFovRH(fov, aspect, near, far);//(glm::perspectiveFov(glm::radians(m_Fov), aspect, 1.0f, m_Near, m_Far));
    //m_ProjectionMatrix = m_ProjectionMatrix.Transpose();
}

const glm::vec3& Camera::GetRotation() const
{
    return m_Rotation;
}

const SimpleMath::Vector3& Camera::GetPosition() const
{
    return m_Position;
}

/*const glm::mat4x4& Camera::GetViewMatrix() const
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
    return glm::vec2(m_Aspect * glm::tan(glm::radians(m_Fov/2.0f)), glm::tan(glm::radians(m_Fov/2.0f)));
}*/
const SimpleMath::Matrix& Camera::GetViewMatrix() const
{
    return m_ViewMatrix;
}

const SimpleMath::Matrix& Camera::GetViewProjectionMatrix() const
{
    return m_ViewProjectionMatrix;
}

const SimpleMath::Matrix& Camera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

glm::vec2 Camera::GetProjectionFactors() const
{
    return glm::vec2(m_Aspect * glm::tan(glm::radians(m_Fov / 2.0f)), glm::tan(glm::radians(m_Fov / 2.0f)));
}
