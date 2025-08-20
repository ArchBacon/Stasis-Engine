#include "Editor.hpp"

#include <SDL3/SDL.h>

#include "Core/ECS.hpp"
#include "Core/Engine.hpp"
#include "Core/Window.hpp"
#include "Graphics/Camera.hpp"

namespace blackbox::editor
{
    Editor::Editor(Window& window)
        : window(window)
    {
        auto& ecs = ::Engine.Container().Get<EntityComponentSystem>();
        camera = ecs.Create<graphics::Camera, EditorType>();
    }

    Editor::~Editor() = default;

    void Editor::Tick(float deltaTime)
    {
        auto& ecs = ::Engine.Container().Get<EntityComponentSystem>();
        auto [cameraEntt, transform] = ecs.Registry().get<graphics::Camera, graphics::Transform>(camera.entity);
        
        const auto keystate = SDL_GetKeyboardState(nullptr);
        float x, y;
        const auto mousestate = SDL_GetMouseState(&x, &y);
        if (keystate[SDL_SCANCODE_W])
        {
            transform.location += cameraMoveSpeed * ::Engine.DeltaTime() * cameraEntt.front;
        }
        if (keystate[SDL_SCANCODE_A])
        {
            transform.location -= glm::normalize(glm::cross(cameraEntt.front, cameraUp)) * cameraMoveSpeed * ::Engine.DeltaTime();
        }
        if (keystate[SDL_SCANCODE_S])
        {
            transform.location -= cameraMoveSpeed * ::Engine.DeltaTime() * cameraEntt.front;
        }
        if (keystate[SDL_SCANCODE_D])
        {
            transform.location += glm::normalize(glm::cross(cameraEntt.front, cameraUp)) * cameraMoveSpeed * ::Engine.DeltaTime();
        }
        if (keystate[SDL_SCANCODE_Q])
        {
            transform.location -= cameraMoveSpeed * ::Engine.DeltaTime() * cameraUp;
        }
        if (keystate[SDL_SCANCODE_E])
        {
            transform.location += cameraMoveSpeed * ::Engine.DeltaTime() * cameraUp;
        }

        if (mousestate & SDL_BUTTON_RMASK)
        {
            SDL_HideCursor();
            SDL_SetWindowMouseGrab(window.window, true);
            SDL_WarpMouseInWindow(window.window, window.Width<float>() / 2, window.Height<float>() / 2);
            
            if (!cameraFirstClick)
            {
                const bool invertY = false;
                float2 relativeMouseMovement
                {
                    (x - window.Width<float>() / 2) / window.Width<float>(),
                    (y - window.Height<float>() / 2) / window.Height<float>()
                };

                const float2 mouseMovement = relativeMouseMovement * cameraRotationSpeed;
                transform.rotation += float3{1.0, 0.0, 0.0} * mouseMovement.x;
                transform.rotation += float3{0.0, 1.0, 0.0} * mouseMovement.y * static_cast<float>(2 * invertY - 1);
                transform.rotation.pitch = glm::clamp(transform.rotation.pitch, -89.f, 89.f);
            }
            cameraFirstClick = false;
        }
        else
        {
            SDL_ShowCursor();
            SDL_SetWindowMouseGrab(window.window, false);
            cameraFirstClick = true;
        }
    }
}
