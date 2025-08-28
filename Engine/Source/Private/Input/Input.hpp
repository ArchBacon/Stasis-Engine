#pragma once

#include <string>
#include <SDL3/SDL_keycode.h>

#include "Types.hpp"

namespace blackbox
{
    class EventBus;

    enum class ActionType : uint8_t
    {
        Digital, // bool
        Axis1D,  // float
        Axis2D,  // float2
    };

    template <typename ParamType>
    struct InputEvent
    {
        std::vector<std::function<void(ParamType)>> onStartedCallbacks {};
        std::vector<std::function<void(ParamType)>> onEndedCallbacks {};
        std::vector<std::function<void(ParamType)>> onTriggeredCallbacks {};
        
        template <typename Class>
        void OnStarted(Class* instance, void(Class::*method)(ParamType));
        template <typename Class>
        void OnEnded(Class* instance, void(Class::*method)(ParamType));
        template <typename Class>
        void OnTriggered(Class* instance, void(Class::*method)(ParamType));
    };

    template <typename ParamType>
    template <typename Class>
    void InputEvent<ParamType>::OnStarted(Class* instance, void(Class::* method)(ParamType))
    {
        auto callback = [instance, method](ParamType value)
        {
            (instance->*method)(static_cast<const ParamType>(value));
        };
        onStartedCallbacks.push_back(callback);
    }

    template <typename ParamType>
    template <typename Class>
    void InputEvent<ParamType>::OnEnded(Class* instance, void(Class::* method)(ParamType))
    {
        auto callback = [instance, method](ParamType value)
        {
            (instance->*method)(static_cast<const ParamType>(value));
        };
        onEndedCallbacks.push_back(callback);
    }

    template <typename ParamType>
    template <typename Class>
    void InputEvent<ParamType>::OnTriggered(Class* instance, void(Class::* method)(ParamType))
    {
        auto callback = [instance, method](ParamType value)
        {
            (instance->*method)(static_cast<const ParamType>(value));
        };
        onTriggeredCallbacks.push_back(callback);
    }
    
    class Input
    {
        EventBus& eventbus;
        
    public:
        Input(EventBus& eventbus);
        ~Input() = default;

        Input(const Input& other) = delete;
        Input& operator=(const Input&) = delete;
        Input(Input&& other) = delete;
        Input& operator=(Input&& other) = delete;

        template <typename T>
        InputEvent<T>& GetEvent(const std::string& name);

        void RegisterAction(std::string name, ActionType type, keys...);
    };

    template <typename T>
    InputEvent<T>& Input::GetEvent(const std::string& name)
    {
        static_assert(std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, float2>, "GetEvent only supports bool, float, and float2");
    }
}
