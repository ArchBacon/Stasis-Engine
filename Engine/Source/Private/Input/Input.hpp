#pragma once

#include <string>
#include <unordered_map>

#include "InputContext.hpp"

namespace blackbox
{
    enum class ActionType : uint8_t;
    struct InputContext;
    struct KeyPressedEvent;
    struct KeyReleasedEvent;
    class EventBus;

    class Input
    {
        EventBus& eventbus;

        // All contexts live on this map on creating until enabled
        std::unordered_map<std::string, InputContext> inactiveContexts {};
        std::unordered_map<std::string, InputContext> activeContexts {};
        
    public:
        Input(EventBus& eventbus);
        ~Input() = default;

        Input(const Input& other) = delete;
        Input& operator=(const Input&) = delete;
        Input(Input&& other) = delete;
        Input& operator=(Input&& other) = delete;

        void Update();
        
        InputContext& GetContext(const std::string& name);
        void EnableContext(const std::string& name);
        void DisableContext(const std::string& name);
        void DisableAllContexts();

    private:
        void OnKeyDown(KeyPressedEvent event);
        void OnKeyUp(KeyReleasedEvent event);
    };
}
