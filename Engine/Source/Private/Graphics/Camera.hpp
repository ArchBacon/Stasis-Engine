#pragma once

#include "Core/Types.hpp"

namespace blackbox::graphics
{
    struct Camera
    {
        float3 location {0.0f, 0.0f, 0.0f};
        float3 rotation {-90.0f, 0.0f, 0.0f};
        float3 worldUp {0.0f, 1.0f, 0.0f};
        float speed {2.5f};

        float3 front {0.0f, 0.0, -3.0};
        float3 up {0.0f, 1.0, 0.0};
        float3 right {1.0f, 0.0, 0.0};

        float fov {45.0f};
        float nearPlane {0.1f};
        float farPlane {100.f};

        glm::mat4 projection {1.0f};
        bool firstClick {true};

        [[nodiscard]] glm::mat4 ViewMatrix() const;
        [[nodiscard]] const glm::mat4& ProjectionMatrix() const { return projection; }
        [[nodiscard]] glm::mat4 ViewProjectionMatrix() const { return projection * ViewMatrix(); }

        void UpdateVectors();
        void RecalculateProjection();
    };
}
