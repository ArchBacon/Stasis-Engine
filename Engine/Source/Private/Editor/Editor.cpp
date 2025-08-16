#include "Editor.hpp"

#include <SDL3/SDL.h>
#include "EditorCamera.hpp"
#include "Core/Engine.hpp"
#include "Core/Window.hpp"

namespace blackbox::editor
{
    Editor::Editor()
    {
        camera = std::make_unique<EditorCamera>();
    }

    Editor::~Editor() = default;

    void Editor::Tick(float deltaTime)
    {
        const auto keystate = SDL_GetKeyboardState(nullptr);
            float x, y;
            const auto mousestate = SDL_GetMouseState(&x, &y);
            if (keystate[SDL_SCANCODE_W])
            {
                camera->location += camera->moveSpeed * ::Engine.DeltaTime() * camera->front;
            }
            if (keystate[SDL_SCANCODE_A])
            {
                camera->location -= glm::normalize(glm::cross(camera->front, camera->up)) * camera->moveSpeed * ::Engine.DeltaTime();
            }
            if (keystate[SDL_SCANCODE_S])
            {
                camera->location -= camera->moveSpeed * ::Engine.DeltaTime() * camera->front;
            }
            if (keystate[SDL_SCANCODE_D])
            {
                camera->location += glm::normalize(glm::cross(camera->front, camera->up)) * camera->moveSpeed * ::Engine.DeltaTime();
            }
            if (keystate[SDL_SCANCODE_Q])
            {
                camera->location -= camera->moveSpeed * ::Engine.DeltaTime() * camera->up;
            }
            if (keystate[SDL_SCANCODE_E])
            {
                camera->location += camera->moveSpeed * ::Engine.DeltaTime() * camera->up;
            }

            if (mousestate & SDL_BUTTON_RMASK)
            {
                SDL_HideCursor();
                SDL_SetWindowMouseGrab(::Engine.Window().window, true);
                SDL_WarpMouseInWindow(::Engine.Window().window, ::Engine.Window().Width<float>() / 2, ::Engine.Window().Height<float>() / 2);
                
                if (!camera->firstClick)
                {
                    const bool invertY = false;
                    float2 relativeMouseMovement {
                        (x - ::Engine.Window().Width<float>() / 2) / ::Engine.Window().Width<float>(),
                        (y - ::Engine.Window().Height<float>() / 2) / ::Engine.Window().Height<float>()
                    };

                    const float2 mouseMovement = relativeMouseMovement * camera->rotationSpeed;
                    camera->AddMovementInput({1.0, 0.0, 0.0}, mouseMovement.x);
                    camera->AddMovementInput({0.0, 1.0, 0.0}, mouseMovement.y * static_cast<float>(2 * invertY - 1));
                }
                camera->firstClick = false;
            }
            else
            {
                SDL_ShowCursor();
                SDL_SetWindowMouseGrab(::Engine.Window().window, false);
                camera->firstClick = true;
            }
    }
}
