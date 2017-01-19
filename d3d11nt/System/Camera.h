#pragma once
#include "Extern/glm/glm.hpp"

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

    void SetProjection(const glm::mat4x4& projectionMatrix);

    const glm::mat4x4& GetViewProjectionMatrix() const;
private:
    void UpdateViewProjectionMatrix();

    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::mat4x4 m_ProjectionMatrix;
    glm::mat4x4 m_ViewProjectionMatrix;
};