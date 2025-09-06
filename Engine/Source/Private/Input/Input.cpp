#include "Input.hpp"

#include "Engine.hpp"
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

    void Input::RemoveAllContexts() { contexts.clear(); }
    
    void Input::OnKeyPressedEvent(const KeyPressedEvent event)
    {
        if (!keybinds.contains({event.key}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.key}];
        if (!contexts.contains(binds->contextType))
        {
            return; // Key does not trigger if the corresponding context isn't active
        }
        
        float2 value {1.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }

        const auto& action = actions[binds->actionType];
        for (auto& callback : action->onStartedCallbacks)
        {
            callback(value);
        }

        activeKeys.insert({event.key});
    }
    
    void Input::OnKeyReleasedEvent(const KeyReleasedEvent event)
    {
        if (!keybinds.contains({event.key}))
        {
            return;
        }
        
        const auto binds = keybinds[{event.key}];
        if (!contexts.contains(binds->contextType))
        {
            return;
        }
        
        float2 value {0.0f, 0.0f};
        for (const auto& mod : binds->modifiers)
        {
            value = mod->Execute(value);
        }
        
        const auto& action = actions[binds->actionType];
        for (auto& callback : action->onEndedCallbacks)
        {
            callback({value});
        }
        
        activeKeys.erase({event.key});
    }
    
    void Input::OnTickEvent(const TickEvent)
    {
        for (auto& key : activeKeys)
        {
            if (!keybinds.contains(key))
            {
                continue;
            }
            
            const auto binds = keybinds[key];
            if (!contexts.contains(binds->contextType))
            {
                continue;
            }
        
            float2 value {1.0f, 0.0f}; // TODO: what is the default key value? this is correct for keyboard, but doesnt work for controllers
            for (const auto& mod : binds->modifiers)
            {
                value = mod->Execute(value);
            }
        
            const auto& action = actions[binds->actionType];
            for (auto& callback : action->onTriggeredCallbacks)
            {
                callback({value});
            }
        } 
    }
}
