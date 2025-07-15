#pragma once

#include "vk_types.h"
#include <SDL3/SDL_events.h>

namespace blackbox
{
    class Camera {
    public:
        float3 velocity {0.0f};
        float3 position {0.0f, 0.0f, -5.f};
        float pitch {0.0f}; // Vertical rotation
        float yaw {0.0f}; // Horizontal rotation

        mat4 GetViewMatrix();
        mat4 GetRotationMatrix();

        void ProcessSdlEvent(const SDL_Event& event);
        void Update();        
    };
}
