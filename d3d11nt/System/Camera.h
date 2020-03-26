#pragma once
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Extern/glm/glm.hpp"
#include "Extern/glm/gtc/matrix_transform.hpp"
#include <d3d11.h>

typedef glm::vec3 CameraPosition;
typedef glm::vec3 CameraRotation;

class OrientationBase
{
public:
    virtual glm::mat4x4 GetTransformation() = 0;
};

class OrientationEuler : public OrientationBase
{
public:
    void SetRotation(const glm::vec3& rot)
    {
        m_Rotation = rot;
    }
    virtual glm::mat4x4 GetTransformation() override
    {
        static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

        glm::mat4x4 transform = glm::rotate(glm::mat4x4(), m_Rotation.x, xAxis);
        transform = glm::rotate(transform, m_Rotation.y, yAxis);
    }
    glm::vec3 m_Rotation;
};

class Camera
{
public:
    Camera(const glm::vec3& position = glm::vec3(), const glm::vec3& rotation = glm::vec3());

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);

    //calling UpdateViewProjectionMatrix once
    void SetPositionRotation(const glm::vec3& position, const glm::vec3& rotation);

    void SetProjection(float fov, float aspect, float near, float far);
    void SetProjectionAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
    void UpdateProjection(float aspect);

    void StepForward(float step);
    void StepLeft(float step);

    void UseAngleParametrization(bool v)
    {
        m_UseAngleParametrization = v;
    }
    void SetEyeX(float v)
    {
        m_ExpEyeX = v;
    }
    void SetEyeY(float v)
    {
        m_ExpEyeY = v;
    }

    const glm::mat4x4& GetViewProjectionMatrix() const;
    const glm::mat4x4& GetViewMatrix() const;
    const glm::mat4x4& GetProjectionMatrix() const;
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetRotation() const;
    glm::vec2 GetProjectionFactors() const;

    void UpdateViewProjectionMatrix();

    //test
    glm::vec3 m_Eye;
private:
    glm::vec3 m_ViewVec;
    glm::vec3 m_LeftVec;
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;

    glm::mat4x4 m_ProjectionMatrix;
    glm::mat4x4 m_ViewMatrix;
    glm::mat4x4 m_ViewProjectionMatrix;

    float m_Fov;
    float m_Near;
    float m_Far;
    float m_Aspect;

    bool m_UseAngleParametrization;
    float m_ExpEyeX;
    float m_ExpEyeY;
};