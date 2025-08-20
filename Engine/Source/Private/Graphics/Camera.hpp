#pragma once

#include "entt/entt.hpp"
#include "Core/DependencyInjection.hpp"
#include "Core/ECS.hpp"
#include "Core/Engine.hpp"
#include "Core/Types.hpp"
#include "Core/Window.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace blackbox::graphics
{
    // Can be rendered
    struct Renderable {};

    // Can move
    struct Transform
    {
        float3 location {0.0f, 0.0f, 0.0f};
        float3 rotation {0.0f, 0.0f, 0.0f};
        float3 scale {0.0f, 0.0f, 0.0f};
    };

    // Is a camera
    struct Camera
    {
        static void on_construct(entt::registry &registry, const entt::entity entt)
        {
            registry.emplace<Transform>(entt);
        }
        
        float fov {45.0f};
        float3 front {0.0f, 0.0f, -1.0f};
        glm::mat4 viewMatrix {1.0f};
        glm::mat4 projectionMatrix {1.0f};
    };

    class CameraSystem
    {
    public:
        void Update(float deltaTime) const
        {
            // TODO: Global config
            float3 cameraUp {0.0f, 1.0f, 0.0f};
            float aspect = ::Engine.Container().Get<Window>().AspectRatio();
            float nearPlane = 0.1f;
            float farPlane = 100.f;
            
            auto& registry = ::Engine.Container().Get<EntityComponentSystem>().Registry();
            const auto view = registry.view<Transform, Camera>();
            for (auto& entity : view)
            {
                auto [transform, camera] = view.get(entity);
                
                // update front (camera rotation)
                camera.front = glm::normalize(float3{
                    std::cos(transform.rotation.pitch) * std::cos(transform.rotation.yaw),
                    std::sin(transform.rotation.pitch),
                    std::cos(transform.rotation.pitch) * std::sin(transform.rotation.yaw)
                });

                // Update view matrix
                camera.viewMatrix = glm::lookAt(transform.location, transform.location + camera.front, cameraUp);

                // update projection matrix
                camera.projectionMatrix = glm::perspective(glm::radians(camera.fov), aspect, nearPlane, farPlane);
            }
        }
    };
}
