#pragma once

#include "Core/Types.hpp"

namespace blackbox::graphics
{
    class Camera
    {
    public:
        float3 location {0.0f, 0.0f, 0.0f};
        float3 rotation {-90.0f, 0.0f, 0.0f};

        float3 front {0.0f, 0.0, -3.0};
        float3 up {0.0f, 1.0, 0.0};
        float3 right {1.0f, 0.0, 0.0};
        float3 worldUp {0.0f, 1.0f, 0.0f};

        float fov {45.0f};
        float nearPlane {0.1f};
        float farPlane {100.f};

        Camera() = default;
        virtual ~Camera() = default;
        
        [[nodiscard]] glm::mat4 ViewMatrix() const;
        [[nodiscard]] glm::mat4 ProjectionMatrix() const;
        [[nodiscard]] glm::mat4 ViewProjectionMatrix() const { return ProjectionMatrix() * ViewMatrix(); }

        virtual void Update();
        void AddMovementInput(float3 axis, float value);
    };
}
