#include "Input.hpp"

#include "Events.hpp"
#include "EventBus.hpp"

namespace blackbox
{
    Input::Input(EventBus& eventbus)
        : eventbus(eventbus)
    {
        eventbus.Subscribe<KeyPressedEvent>(this, &Input::OnKeyPressedEvent);
        eventbus.Subscribe<KeyReleasedEvent>(this, &Input::OnKeyReleasedEvent);
        eventbus.Subscribe<TickEvent>(this, &Input::OnTickEvent);
    }

    void Input::Clear()
    {
        contexts.clear();
    }
    
    void Input::OnKeyPressedEvent(const KeyPressedEvent event)
    {
        event.key;
    }

    void Input::OnKeyReleasedEvent(const KeyReleasedEvent event)
    {
        event.key;
    }

    void Input::OnTickEvent(const TickEvent event)
    {
        event.deltaTime;
    }
}
