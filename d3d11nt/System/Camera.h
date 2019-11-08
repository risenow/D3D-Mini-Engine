#pragma once
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Extern/glm/glm.hpp"
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;

typedef glm::vec3 CameraPosition;
typedef glm::vec3 CameraRotation;

class Camera
{
public:
    Camera(const glm::vec3& position = glm::vec3(), const glm::vec3& rotation = glm::vec3());

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);

    //calling UpdateViewProjectionMatrix once
    void SetPositionRotation(const glm::vec3& position, const glm::vec3& rotation);

    void SetProjection(float fov, float aspect, float near, float far);
    void UpdateProjection(float aspect);

    void StepForward(float step);
    void StepLeft(float step);

    const glm::mat4x4& GetViewProjectionMatrix() const;
    const glm::mat4x4& GetViewMatrix() const;
    const glm::mat4x4& GetProjectionMatrix() const;
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetRotation() const;
    glm::vec2 GetProjectionFactors() const;

    void UpdateViewProjectionMatrix();
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
};