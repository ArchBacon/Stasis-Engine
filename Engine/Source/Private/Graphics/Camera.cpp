#include "Camera.hpp"

#include "Core/Engine.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace blackbox::graphics
{
    Camera::Camera(const float aspectRatio)
        : aspectRatio{aspectRatio}
    {}

    glm::mat4 Camera::ViewMatrix() const
    {
        return glm::lookAt(location, location + front, up);
    }

    glm::mat4 Camera::ProjectionMatrix() const
    {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void Camera::Update()
    {
        const float3 rotationRad = glm::radians(rotation);
        front = glm::normalize(float3{
            std::cos(rotationRad.pitch) * std::cos(rotationRad.yaw),
            std::sin(rotationRad.pitch),
            std::cos(rotationRad.pitch) * std::sin(rotationRad.yaw)
        });
    }

    void Camera::AddMovementInput(const float3 axis, const float value)
    {
        rotation += axis * value;
        rotation.pitch = glm::clamp(rotation.pitch, -89.f, 89.f);
    }
}
