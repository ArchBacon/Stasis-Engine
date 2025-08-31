#pragma once

#include <vector>
#include <functional>

#include "InputKeys.hpp"

namespace blackbox
{
    template <typename ParamType>
    struct InputAction
    {
        friend class Input;
        friend struct InputContext;
        
    private:
        std::vector<std::function<void(ParamType)>> onStartedCallbacks {};
        std::vector<std::function<void(ParamType)>> onEndedCallbacks {};
        std::vector<std::function<void(ParamType)>> onTriggeredCallbacks {};

        using type = ParamType;
        bool active {false};
        
    public:
        std::string name {};
        std::vector<InputKey> keys {};

        template <typename Class>
        void OnStarted(Class* instance, void (Class::*method)(ParamType));
        template <typename Class>
        void OnEnded(Class* instance, void (Class::*method)(ParamType));
        template <typename Class>
        void OnTriggered(Class* instance, void (Class::*method)(ParamType));
    };

    template <typename ParamType>
    template <typename Class>
    void InputAction<ParamType>::OnStarted(Class* instance, void (Class::*method)(ParamType))
    {
        auto callback = [instance, method](ParamType value)
        {
            (instance->*method)(static_cast<const ParamType>(value));
        };
        onStartedCallbacks.push_back(callback);
    }

    template <typename ParamType>
    template <typename Class>
    void InputAction<ParamType>::OnEnded(Class* instance, void (Class::*method)(ParamType))
    {
        auto callback = [instance, method](ParamType value)
        {
            (instance->*method)(static_cast<const ParamType>(value));
        };
        onEndedCallbacks.push_back(callback);
    }

    template <typename ParamType>
    template <typename Class>
    void InputAction<ParamType>::OnTriggered(Class* instance, void (Class::*method)(ParamType))
    {
        auto callback = [instance, method](ParamType value)
        {
            (instance->*method)(static_cast<const ParamType>(value));
        };
        onTriggeredCallbacks.push_back(callback);
    }
}
