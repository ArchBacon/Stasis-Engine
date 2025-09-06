#pragma once

#include "Events.hpp"
#include "SDL3InputHelper.hpp"

namespace blackbox
{
    std::string to_string(const SDL_Event& event);
    std::string to_string(SDL_EventType eventType);
    
    // Convert SDL events into Blackbox events and dispatch those
    class SDL3ToBlackBoxEvent
    {
    public:
        static void Broadcast(SDL_Event& e, EventBus& eventbus)
        {
            switch (static_cast<SDL_EventType>(e.type))
            {
            case SDL_EVENT_QUIT:
                eventbus.Broadcast(ShutdownEvent {});
                break;
            case SDL_EVENT_WINDOW_MINIMIZED:
                eventbus.Broadcast(WindowMinimizedEvent {});
                break;
            case SDL_EVENT_WINDOW_RESTORED:
                eventbus.Broadcast(WindowRestoredEvent {});
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                eventbus.Broadcast(WindowResizedEvent {.windowSize = {e.window.data1, e.window.data2}});
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                eventbus.Broadcast(WindowFocusLostEvent {});
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                eventbus.Broadcast(WindowFocusGainedEvent {});
                break;
            case SDL_EVENT_KEY_DOWN:
                e.key.repeat
                    ? eventbus.Broadcast(KeyRepeatEvent {.key = MapSDLKeyboard(e.key.scancode) })
                    : eventbus.Broadcast(KeyPressedEvent {.key = MapSDLKeyboard(e.key.scancode) });
                break;
            case SDL_EVENT_KEY_UP:
                eventbus.Broadcast(KeyReleasedEvent {.key = MapSDLKeyboard(e.key.scancode) });
                break;
            case SDL_EVENT_MOUSE_MOTION:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                // TODO: implement
                break;
            default:
                // LogEngine->Warn("Event {} not mapped.", to_string(e));
                break;
            }
        }
    };
}
