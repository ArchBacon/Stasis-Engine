#pragma once

#include <SDL3/SDL_keycode.h>

#include "Types.hpp"

namespace blackbox
{
    struct Event {};
    
    // Engine Events
    struct QuitEvent : Event {};

    // Input Events
    struct KeyDownEvent : Event { SDL_Keycode key {}; };
    struct KeyUpEvent : Event { SDL_Keycode key {}; };

    // Window Events
    struct WindowMinimizedEvent : Event {};
    struct WindowRestoredEvent : Event {};
    struct WindowResizedEvent : Event { float2 newWindowSize {}; };
    struct WindowFocusLostEvent : Event {};
    struct WindowFocusGainedEvent : Event {};
}
