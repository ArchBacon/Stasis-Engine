#include "camera.h"
#include "Core/Types.hpp"

glm::mat4 blackbox::Camera::GetViewMatrix()
{
    // To create a correct model view, we need to move the world in opposite direction to the camera.
    // So we will create the camera model matrix and invert
    mat4 cameraTranslation = translate(mat4(1.0f), position);
    mat4 cameraRotation = GetRotationMatrix();
    return inverse(cameraTranslation * cameraRotation);
}

glm::mat4 blackbox::Camera::GetRotationMatrix()
{
    // Fairly typical FPS style camera. We join the pitch and yaw rotations into the final rotation matrix
    quat pitchRotation = angleAxis(pitch, float3(1.0f, 0.0f, 0.0f));
    quat yawRotation = angleAxis(yaw, float3(0.0f, -1.0f, 0.0f));

    return mat4_cast(yawRotation) * mat4_cast(pitchRotation);
}

void blackbox::Camera::ProcessSdlEvent(
    const SDL_Event& event
) {
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (event.key.key == SDLK_W) { velocity.z = -1; }
        if (event.key.key == SDLK_S) { velocity.z = 1; }
        if (event.key.key == SDLK_A) { velocity.x = -1; }
        if (event.key.key == SDLK_D) { velocity.x = 1; }
        if (event.key.key == SDLK_Q) { velocity.y = -1; }
        if (event.key.key == SDLK_E) { velocity.y = 1; }
    }

    if (event.type == SDL_EVENT_KEY_UP)
    {
        if (event.key.key == SDLK_W) { velocity.z = 0; }
        if (event.key.key == SDLK_S) { velocity.z = 0; }
        if (event.key.key == SDLK_A) { velocity.x = 0; }
        if (event.key.key == SDLK_D) { velocity.x = 0; }
        if (event.key.key == SDLK_Q) { velocity.y = 0; }
        if (event.key.key == SDLK_E) { velocity.y = 0; }
    }

    // If left mouse button is pressed
    if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK)
    {
        // Rotate the camera based on mouse movement
        if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            yaw += (float)event.motion.xrel / 200.f;
            pitch += -((float)event.motion.yrel / 200.f);
        }
    }
}

void blackbox::Camera::Update()
{
    const mat4 cameraRotation = GetRotationMatrix();
    position += float3(cameraRotation * float4(velocity * 0.5f, 0.0f));
}
