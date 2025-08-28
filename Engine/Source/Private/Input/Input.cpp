#include "Input.hpp"

#include "EventBus.hpp"

namespace blackbox
{
    Input::Input(EventBus& eventbus)
        : eventbus(eventbus)
    {
        eventbus.Subscribe<KeyDownEvent>(this, &Input::OnKeyDown)
    }

    void Input::OnKeyDown(const SDL_Keycode key, const std::function<void()>&)
    {
        
    }
}
