#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#include "Blackbox.hpp"
#include "InputAction.hpp"
#include "Types.hpp"

namespace blackbox
{
    using ActionValue = std::variant<
        InputAction<bool>,
        InputAction<float>,
        InputAction<float2>
    >;

    struct InputContext
    {
        friend class Input;
        
    private:
        std::unordered_map<std::string, ActionValue> actions {};
        std::string name {};

    public:
        template <typename T>
        InputAction<T>& GetAction(const std::string& actionName);

        template <typename T>
        void AddAction(const std::string& actionName, const std::vector<InputKey>& keys);
    };

    template <typename T>
    void InputContext::AddAction(const std::string& actionName, const std::vector<InputKey>& keys)
    {
        static_assert(std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, float2>, "AddAction only supports bool, float, and float2");
        
        InputAction<T> action {};
        action.name = actionName;
        action.keys = keys;
        actions[actionName] = action;
    }

    template <typename T>
    InputAction<T>& InputContext::GetAction(const std::string& actionName)
    {
        static_assert(std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, float2>, "GetAction only supports bool, float, and float2");

        if (!actions.contains(actionName))
        {
            LogEngine->Warn("Action `{}` not registered on context `{}`.", actionName, this->name);
        }

        return std::get<InputAction<T>>(actions[actionName]);
    }
}
