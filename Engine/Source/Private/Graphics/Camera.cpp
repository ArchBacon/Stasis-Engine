#include "Camera.hpp"

#include "glm/ext/matrix_transform.hpp"

namespace blackbox::graphics
{
    glm::mat4 Camera::ViewMatrix() const
    {
        return glm::lookAt(location, location + front, up);
    }

    void Camera::UpdateVectors()
    {
        const float3 rotationRad = glm::radians(rotation);
        front = glm::normalize(float3{
            std::cos(rotationRad.pitch) * std::cos(rotationRad.yaw),
            std::sin(rotationRad.pitch),
            std::cos(rotationRad.pitch) * std::sin(rotationRad.yaw)
        });

        
    }

    void Camera::RecalculateProjection()
    {
    }
}
